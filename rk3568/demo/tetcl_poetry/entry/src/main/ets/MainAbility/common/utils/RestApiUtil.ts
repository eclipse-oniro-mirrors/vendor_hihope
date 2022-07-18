/**
 * @Description 返回数据工具
 * @date 2022/7/18 9:41
 * @Author Bai XiaoMing
 * @Version 1.0
 */
export default class RestApiUtil {
    code: number;
    msg: string;
    newslist: Array<any>;

    constructor(code: number, msg: string, newslist: Array<any>) {
        this.code = code;
        this.msg = msg;
        this.newslist = newslist;
    }
}