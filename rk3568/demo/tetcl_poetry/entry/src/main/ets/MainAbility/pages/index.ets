/*
 * Copyright (C) 2022 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */
import prompt from '@ohos.prompt';
import request from '../common/utils/RequestUtil';
import log from '../common/utils/LogUtil';
import str from '../common/utils/StrUtil';
import RestApi from '../common/utils/RestApiUtil';
import Constant from '../common/constant/Constant';

const TAG = "Index";


@Entry
@Component
struct Index {
  @State poetryArray: Array<any> = new Array<any>();

  @State word: string = "";
  @State len: number = 0;
  @State type: number = 1;
  @State pat: number = 1;

  @State lenSelect: number = 0;
  @State typeSelect: number = 0;
  @State patSelect: number = 0;

  private textInputController: TextInputController = new TextInputController();
  private swiperController: SwiperController = new SwiperController();

  getPoetry = () => {
    this.poetryArray = new Array<any>();
    let url = 'https://api.tianapi.com/cangtoushi/index';
    url += `?key=${Constant.RequestApiKey}`;
    url += `&word=${this.word}`;
    url += `&len=${this.len}`;
    url += `&type=${this.type}`;
    url += `&pat=${this.pat}`;
    request.get(url).then((res) => {
      log.printInfo(TAG, JSON.stringify(res));
      let data: RestApi = res;
      if (data.code === 250) {
        prompt.showToast({
          message: data.msg,
          duration: 2000
        })
      } else if (data.code === 200) {
        this.poetryArray = data.newslist;
      } else {
        prompt.showToast({
          message: data.msg,
          duration: 2000
        })
      }
    })
  }

  build() {
    Flex({direction: FlexDirection.Column}) {
      Column({space: 20}) {
        Row() {
          Text('藏头诗').fontColor($r('app.color.main_font_color')).fontSize(18).fontWeight(FontWeight.Bold)
        }
        .width('100%').height(54)
        .justifyContent(FlexAlign.Center)
        Column() {
          Column({space: 20}) {
            Flex() {
              TextInput({placeholder: "请输入藏字内容...", controller: this.textInputController})
                .height(64)
                .placeholderColor($r('app.color.tips_font_color'))
                .placeholderFont({size: 18, weight: 400})
                .backgroundColor(0xffffff)
                .onChange((value) => {
                  this.word = value;
                })
            }
            Column({space: 4}) {
              Text('点击按钮配置诗句指标：')
                .fontSize(14).fontColor($r('app.color._font_color'))
              Divider().color($r('app.color.white')).strokeWidth(1);
            }
            .alignItems(HorizontalAlign.Start)
            .width('100%')
            Row({space: 6}) {
              Button("诗句格式")
                .fontSize(14)
                .backgroundColor(0xfeb92e)
                .onClick(() => {
                  TextPickerDialog.show({
                    range: Constant.PoetryFormat,
                    selected: this.lenSelect,
                    onAccept: (value: TextPickerResult) => {
                      this.lenSelect = value.index;
                      if (value.index === 0) this.len = 0;
                      else this.len = value.index - 1;
                    }
                  })
                })
              Button("藏头位置")
                .fontSize(14)
                .backgroundColor(0xfc6e00)
                .onClick(() => {
                  TextPickerDialog.show({
                    range: Constant.PoetryType,
                    selected: this.typeSelect,
                    onAccept: (value: TextPickerResult) => {
                      this.typeSelect = value.index;
                      if (value.index === 0) this.type = 1;
                      else this.type = value.index;
                    }
                  })
                })
              Button("押韵类型")
                .fontSize(14)
                .backgroundColor(0x0ed8b0)
                .onClick(() => {
                  TextPickerDialog.show({
                    range: Constant.PoetryPat,
                    selected: this.patSelect,
                    onAccept: (value: TextPickerResult) => {
                      this.patSelect = value.index;
                      if (value.index === 0) this.pat = 0;
                      else this.pat = value.index;
                    }
                  })
                })
            }
            .width('100%')
            Flex({justifyContent: FlexAlign.End}) {
              Button('生成')
                .width(200).height(60)
                .fontSize(16).fontWeight(FontWeight.Bold)
                .backgroundColor(0x007aff)
                .onClick(() => {
                  if (str.isEmpty(this.word)) {
                    prompt.showToast({
                      message: "生成藏头诗内容不能为空!",
                      duration: 2000
                    })
                  } else if (this.word.length < 2 || this.word.length > 8) {
                    prompt.showToast({
                      message: "生成藏头诗内容长度为2-8字符!",
                      duration: 2000
                    })
                  } else {
                    this.getPoetry();
                  }
                })
            }
            .width('100%')
          }
          .width('90%')
          .padding(16)
          .borderRadius(16)
          .backgroundColor(0xf2f2f2)
        }
        .width('100%')

        if (this.poetryArray) {
          Swiper(this.swiperController) {
            ForEach(this.poetryArray, (item: any, index: number) => {
              Text(item.list)
                .width('90%').height(160)
                .linearGradient({
                  angle: 135,
                  direction: GradientDirection.LeftBottom,
                  colors: Constant.BgColors[index]
                })
                .margin(10)
            }, (item: any) => item.list)
          }
          .autoPlay(true)
          .loop(true)
        }
      }
      .width('100%').height('100%')
    }
    .width('100%').height('100%')
  }

  aboutToAppear() {

  }
}