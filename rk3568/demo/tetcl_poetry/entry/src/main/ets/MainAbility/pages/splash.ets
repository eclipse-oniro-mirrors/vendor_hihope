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