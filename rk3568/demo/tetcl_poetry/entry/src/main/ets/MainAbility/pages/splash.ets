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
import router from '@ohos.router';
@Entry
@Component
struct Splash {

  build() {
    Flex({direction: FlexDirection.Column}) {
      Flex({justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center}) {
        Image($r('app.media.icon'))
          .width(120).height(120)
      }
      .layoutWeight(2)
      Column({space: 10}) {
        Text('字中含意，意中有情')
          .fontSize(24).fontWeight(FontWeight.Bold).fontColor($r('app.color.main_font_color'))
        Text('The meaning of the word is sentimental')
          .fontSize(18).fontWeight(FontWeight.Bold).fontColor($r('app.color._font_color'))
      }
      .layoutWeight(1)
    }
    .width('100%').height('100%')
  }

  aboutToAppear() {
    setTimeout(() => {
      router.replace({
        url: 'pages/index'
      })
    }, 3000)
  }
}