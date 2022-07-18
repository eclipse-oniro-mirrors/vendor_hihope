/**
 * @Description 字符串处理工具
 * @date 2022/7/18 14:20
 * @Author Bai XiaoMing
 * @Version 1.0
 */

export default class StrUtil {
    /**
     * 是否为空
     * @param str
     */
    static isEmpty(str: string) {
        return str === null || str === "";
    }

    /**
     * 不为空
     * @param str
     */
    static isNotEmpty(str: string) {
        return !this.isEmpty(str);
    }

    /**
     * 是否存在空
     * @param strArr
     */
    static isAnyEmpty(...strArr: any) {
        if (Array.length <= 0) {
            return false;
        }
        for (const str in strArr) {
            if (this.isEmpty(strArr[str])) {
                return true;
            }
        }
        return false;
    }

    /**
     * 所有值不为空
     * @param strArr
     */
    static isNoneEmpty(...strArr: any) {
        return !this.isAnyEmpty(strArr);
    }

    /**
     * 是否为真空
     * @param str
     */
    static isBlank(str: string) {
        return str === null || /^\s*$/.test(str);
    }

    /**
     * 不为真空
     * @param str
     */
    static isNotBlank(str: string) {
        return !this.isBlank(str);
    }

    /**
     * 是否存在真空
     * @param strArr
     */
    static isAnyBlank(...strArr: any) {
        if (Array.length <= 0) {
            return false;
        }
        for (const str in strArr) {
            if (this.isBlank(strArr[str])) {
                return true;
            }
        }
        return false;
    }

    /**
     * 所有值不为真空
     * @param strArr
     */
    static isNoneBlank(...strArr: any) {
        return !this.isAnyBlank(strArr);
    }
}