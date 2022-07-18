/**
 * @Description 常量
 * @date 2022/7/18 12:15
 * @Author Bai XiaoMing
 * @Version 1.0
 */

export default class Constant {
    static readonly RequestApiKey: string = "天行数据分配给你的Key值";

    static readonly BgColors: Array<any> = [
        [[0xFF6CAB, 0.0], [0x7366FF, 1.0]],
        [[0x64EBDE, 0.0], [0x8A64EB, 1.0]],
        [[0xFFCDA5, 0.0], [0xEE4D5F, 1.0]]
    ];

    static readonly PoetryFormat = ['诗句格式','五言诗', '七言诗'];

    static readonly PoetryType = ['藏头位置','藏头', '藏尾', '藏中', '递增', '递减'];

    static readonly PoetryPat = ['押韵类型','双句一压', '双句押韵', '一三四押'];
}