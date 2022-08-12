/**
 * @Description 请求参数
 * @date 2022/7/18 10:18
 * @Author Bai XiaoMing
 * @Version 1.0
 */
export default class Param {
    key: string;
    word: string;
    len?: number;
    type?: number;
    pat?: number;

    constructor(word: string, len?: number, type?: number, pat?: number) {
        this.key = "04dc9e1ef3a8b6308f640331d81e032c";
        this.word = word;
        this.len = len;
        this.type = type;
        this.pat = pat;
    }
}