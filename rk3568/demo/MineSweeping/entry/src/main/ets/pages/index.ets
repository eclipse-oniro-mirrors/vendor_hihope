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
class Board {
  x: number
  y: number
  content: string
  isCover: boolean
  isMine: boolean
  isClick: boolean

  constructor(x: number, y: number, content: string, isCover: boolean, isMine: boolean, isClick: boolean) {
    this.x = x;
    this.y = y;
    this.content = content;
    this.isCover = isCover;
    this.isMine = isMine;
    this.isClick = isClick;
  }
}


@Entry
@Component
struct Index {
  @State boards: Array<Board> = new Array<Board>();
  // 当前设置雷计数
  @State mineCount: number = 0;
  // 最大雷数
  private maxMineNum: number = 4;

  // 棋盘行/列数
  private boardRowsNum: number = 4;
  private boardColsNum: number = 4;

  @State images: Array<Resource> = [$r("app.media.app_icon"), $r('app.media.loading_icon')]
  @State gridFr: string = "";
  @State clickCount: number = 0;

  // 初始化棋盘行显示数
  initGrid = (rows: number) => {
    for (let i = 0; i < rows; i++) {
      this.gridFr += (i === 0 ? '1fr' : ' 1fr');
    }
  }

  init = () => {
    this.initBoard(this.boardRowsNum, this.boardColsNum);
    this.setMine(this.boardRowsNum, this.boardColsNum);
    this.boardAreaMine(this.boardRowsNum, this.boardColsNum);
  }

  // 初始化棋盘
  initBoard = (rows: number, cols: number) => {
    this.boards = new Array<Board>();
    this.clickCount = rows * cols;
    this.mineCount = 0;
    this.gridFr = "";
    this.initGrid(rows);
    for (let i = 0; i < rows; i++) {
      for (let j = 0; j < cols; j++) {
        this.boards.push(new Board(i, j, '0', true, false, false))
      }
    }
  }

  // 埋雷
  setMine = (rows: number, cols: number) => {
    // 当达到设定的数量时跳出
    if (this.mineCount >= this.maxMineNum) {
      return false;
    }
    // 随机获取坐标值
    let randomX = Math.floor(Math.random() * rows);
    let randomY = Math.floor(Math.random() * cols);
    // 埋雷
    this.boards.forEach(item => {
      if (item.x === randomX && item.y === randomY) {
        if (!item.isMine) {
          item.isMine = true;
          this.mineCount++;
        }
      }
    })
    this.setMine(rows, cols);
  }

  // 统计周边雷数
  boardAreaMine = (rows: number, cols: number) => {
    // 判断周边雷，并计数
    let boards = this.boards;
    for (let i = 0; i < boards.length; i++) {
      let cell = boards[i];
      if (cell.isMine) {
        continue;
      }
      let count = 0;
      // 左上
      let leftTopCellX = cell.x - 1, leftTopCellY = cell.y - 1;
      if (leftTopCellX >= 0 && leftTopCellY >= 0 && leftTopCellX < rows && leftTopCellY < cols) {
        boards.filter(item => {
          if (item.x === leftTopCellX && item.y === leftTopCellY && item.isMine) {
            count++;
          }
        })
      }
      // 上
      let topCellX = cell.x - 1, topCellY = cell.y;
      if (topCellX >= 0 && topCellY >= 0 && topCellX < rows && topCellY < cols) {
        boards.filter(item => {
          if (item.x === topCellX && item.y === topCellY && item.isMine) {
            count++;
          }
        })
      }
      // 右上
      let rightTopCellX = cell.x - 1, rightTopCellY = cell.y + 1;
      if (rightTopCellX >= 0 && rightTopCellY >= 0 && rightTopCellX < rows && rightTopCellY < cols) {
        boards.filter(item => {
          if (item.x === rightTopCellX && item.y === rightTopCellY && item.isMine) {
            count++;
          }
        })
      }
      // 右
      let rightCellX = cell.x, rightCellY = cell.y + 1;
      if (rightCellX >= 0 && rightCellY >= 0 && rightCellX < rows && rightCellY < cols) {
        boards.filter(item => {
          if (item.x === rightCellX && item.y === rightCellY && item.isMine) {
            count++;
          }
        })
      }
      // 右下
      let rightBottomCellX = cell.x + 1, rightBottomCellY = cell.y + 1;
      if (rightBottomCellX >= 0 && rightBottomCellY >= 0 && rightBottomCellX < rows && rightBottomCellY < cols) {
        boards.filter(item => {
          if (item.x === rightBottomCellX && item.y === rightBottomCellY && item.isMine) {
            count++;
          }
        })
      }
      // 下
      let bottomCellX = cell.x + 1, bottomCellY = cell.y;
      if (bottomCellX >= 0 && bottomCellY >= 0 && bottomCellX < rows && bottomCellY < cols) {
        boards.filter(item => {
          if (item.x === bottomCellX && item.y === bottomCellY && item.isMine) {
            count++;
          }
        })
      }
      // 左下
      let leftBottomCellX = cell.x + 1, leftBottomCellY = cell.y - 1;
      if (leftBottomCellX >= 0 && leftBottomCellY >= 0 && leftBottomCellX < rows && leftBottomCellY < cols) {
        boards.filter(item => {
          if (item.x === leftBottomCellX && item.y === leftBottomCellY && item.isMine) {
            count++;
          }
        })
      }
      // 左
      let leftCellX = cell.x, leftCellY = cell.y - 1;
      if (leftCellX >= 0 && leftCellY >= 0 && leftCellX < rows && leftCellY < cols) {
        boards.filter(item => {
          if (item.x === leftCellX && item.y === leftCellY && item.isMine) {
            count++;
          }
        })
      }
      if (count === 0) {
        count = 9;
      }
      cell.content = count.toString();
    }
    this.boards = boards;
  }


  build() {
    Column({space: 20}) {
      Row() {
        Stack({alignContent: Alignment.Center}) {
          Image($r('app.media.start_game'))
            .width(240)
            .height(120)
          Text('开始游戏')
            .fontSize(18)
            .fontColor(Color.White)
            .fontWeight(FontWeight.Bold)
        }
        .onClick(() => {
          this.init();
        })
      }


      Grid() {
        ForEach(this.boards, (item: Board) => {
          GridItem() {
            Stack({alignContent: Alignment.Center}) {
              Image(item.isCover ? $r('app.media.loading_icon') : (item.isMine ? $r('app.media.app_icon') : $r('app.media.click_bg')))
                .width((!item.isCover && item.isMine) ? 80 : '100%')
              Text(item.isClick ? ((item.content === '9' || item.content === '0') ? '' : item.content) : '')
                .fontSize(26).fontWeight(FontWeight.Bold)
            }
            .width('100%').height(100)
          }
          .border({
            width: 1,
            style: BorderStyle.Solid,
            color: Color.White
          })
          .onClick(() => {
            if ((this.clickCount - 1) === this.maxMineNum) {
              prompt.showToast({
                message: '恭喜你，成功排雷！',
                duration: 2000
              })
              this.boards = [];
              return false;
            }
            let tempBoards = this.boards;
            this.boards = new Array<Board>();
            tempBoards.forEach(temp => {
              if (temp.x === item.x && temp.y === item.y) {
                temp.isClick = true;
                temp.isCover = false
                if (temp.isMine) {
                  AlertDialog.show({
                    message: '您踩雷了，游戏结束~',
                    autoCancel: false,
                    primaryButton: {
                      value: '重新开始',
                      action: () => {
                        this.init();
                      }
                    },
                    secondaryButton: {
                      value: '不玩了~',
                      action: () => {
                        this.boards = [];
                      }
                    },
                    alignment: DialogAlignment.Center
                  })
                } else {
                  this.clickCount--;
                }
              }
            })
            this.boards = tempBoards;
          })
        }, (item: Board) => (item.x + ',' + item.y).toString())
      }
      .width('95%')
      .columnsTemplate(this.gridFr)
      .columnsGap(0)
      .rowsGap(0)
      .height(500)

      if (this.boards.length > 0) {
        Row() {
          Text(`还剩多少方格${this.clickCount}`).fontSize(16).fontWeight(FontWeight.Bold)
          Text(`总共有${this.maxMineNum}个雷需要排`).fontSize(16).fontWeight(FontWeight.Bold)
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceAround)
      }
    }
    .width('100%')
    .height('100%')
  }
}