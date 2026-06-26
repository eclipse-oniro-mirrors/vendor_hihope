#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
在编译前自动应用 vendor 仓的 patch 到 SOC 仓
"""

import os
import sys
import subprocess

def apply_patches():
    """应用所有 patch 文件"""
    # 获取当前脚本所在目录（vendor/hihope/nearlink_dk_3863_xts_minimal）
    vendor_dir = os.path.dirname(os.path.abspath(__file__))
    
    # SOC 仓根目录
    soc_dir = os.path.join(vendor_dir, '..', '..', '..', 'device', 'soc', 'hisilicon', 'ws63v100')
    soc_dir = os.path.normpath(soc_dir)
    
    if not os.path.exists(soc_dir):
        print(f"[ERROR] SOC directory not found: {soc_dir}")
        return False
    
    # 检查 SOC 仓是否有未提交的修改
    print(f"[INFO] Checking SOC repository status...")
    cmd = ['git', 'status', '--porcelain']
    result = subprocess.run(cmd, cwd=soc_dir, capture_output=True, text=True)
    
    if result.stdout.strip():
        print(f"[WARN] SOC repository has uncommitted changes:")
        print(result.stdout)
        print(f"[INFO] Attempting to reset SOC repository to clean state...")
        
        # 尝试重置所有修改
        cmd = ['git', 'checkout', '.']
        result = subprocess.run(cmd, cwd=soc_dir, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"[ERROR] Failed to reset SOC repository: {result.stderr}")
            return False
        
        print(f"[SUCCESS] SOC repository reset successfully")
    else:
        print(f"[INFO] SOC repository is clean")
    
    # patches 目录
    patches_dir = os.path.join(vendor_dir, 'patches')
    
    # 收集所有 patch 文件
    patch_files = []
    
    # 1. 从 vendor/patches 目录收集（排除空文件）
    if os.path.exists(patches_dir):
        for f in os.listdir(patches_dir):
            if f.endswith('.patch'):
                patch_path = os.path.join(patches_dir, f)
                # 跳过空文件
                if os.path.getsize(patch_path) > 0:
                    patch_files.append(patch_path)
    
    if not patch_files:
        print(f"[INFO] No patch files found")
        return True
    
    print(f"[INFO] Found {len(patch_files)} patch file(s)")
    
    # 应用每个 patch
    for patch_path in sorted(patch_files):
        patch_name = os.path.basename(patch_path)
        print(f"[INFO] Applying patch: {patch_name}")
        
        try:
            # 检查 patch 是否需要 -p1 参数（去除第一层目录）
            # 如果 patch 中的路径是 ws63v100/sdk/...，而我们在 ws63v100 目录下，需要 -p1
            needs_strip = False
            with open(patch_path, 'r', encoding='utf-8', errors='ignore') as f:
                for line in f:
                    if line.startswith('diff --git a/'):
                        # 检查路径是否以 ws63v100/ 开头
                        if 'ws63v100/' in line:
                            needs_strip = True
                        break
            
            # 使用 git apply 应用 patch（先检查，再应用）
            if needs_strip:
                check_cmd = ['git', 'apply', '--check', '-p1', patch_path]
                apply_cmd = ['git', 'apply', '-p1', '--ignore-whitespace', patch_path]
            else:
                check_cmd = ['git', 'apply', '--check', patch_path]
                apply_cmd = ['git', 'apply', '--ignore-whitespace', patch_path]
            
            print(f"[DEBUG] Working directory: {soc_dir}")
            print(f"[DEBUG] Check command: {' '.join(check_cmd)}")
            result = subprocess.run(check_cmd, cwd=soc_dir, capture_output=True, text=True)
            print(f"[DEBUG] Check return code: {result.returncode}")
            
            if result.returncode != 0:
                # patch 可能已经应用过了，尝试跳过
                print(f"[WARN] Patch check failed, trying to apply anyway...")
                result = subprocess.run(apply_cmd, cwd=soc_dir, capture_output=True, text=True)
                
                if result.returncode != 0:
                    print(f"[ERROR] Failed to apply patch: {patch_name}")
                    print(f"[ERROR] stdout: {result.stdout}")
                    print(f"[ERROR] stderr: {result.stderr}")
                    return False
            else:
                # 检查通过，正式应用 patch
                print(f"[DEBUG] Check passed, applying patch...")
                print(f"[DEBUG] Apply command: {' '.join(apply_cmd)}")
                result = subprocess.run(apply_cmd, cwd=soc_dir, capture_output=True, text=True)
                print(f"[DEBUG] Apply return code: {result.returncode}")
                
                if result.returncode != 0:
                    print(f"[ERROR] Failed to apply patch: {patch_name}")
                    print(f"[ERROR] stdout: {result.stdout}")
                    print(f"[ERROR] stderr: {result.stderr}")
                    return False
            
            print(f"[SUCCESS] Applied: {patch_name}")
            
        except Exception as e:
            print(f"[ERROR] Exception while applying patch {patch_name}: {e}")
            return False
    
    print(f"[INFO] All patches applied successfully")
    return True

if __name__ == '__main__':
    success = apply_patches()
    
    # 创建虚拟输出文件（GN 要求 action 必须有 outputs）
    if success and len(sys.argv) > 1:
        output_file = sys.argv[1]
        output_dir = os.path.dirname(output_file)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir, exist_ok=True)
        with open(output_file, 'w') as f:
            f.write(f"Patches applied at {__import__('datetime').datetime.now()}\n")
    
    sys.exit(0 if success else 1)
