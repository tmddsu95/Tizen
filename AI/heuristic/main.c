#include <tetris5.h>
#include <app.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <dlog.h>
#include <stdlib.h>

typedef struct _UIData {
   Evas_Object *win;
   Evas_Object *conform;
   Evas_Object *grid;
   Evas_Object *rect_table; // 테트리스 판을 만들기 위한 테이블_사각형들을 배치한다
   Evas_Object *next_block_table; // 다음 블록을 보여주기 위한 테이블 _ 우측 상단 위치
   Evas_Object *score_label; // 점수를 표시하는 레이블
   Evas_Object *rect[240]; // 테트리스 판을 이루는 사각형들
   Evas_Object *next_rect[12]; // next block을 보여주는 사각형들
   Evas_Object *popup; // 팝업창을 띄우기 위한 오브젝트
   Evas_Object *popup_box; // 팝업창 위젯을 순서대로 배치
   int timer_count;
} UIData;

Ecore_Timer *timer1; // 1초마다 동작하는 타이머
int pos; // 블록을 그리는 기준이 되는 점
int shape=0; // 블록의 모양을 바꾸는 변수
int rotation=0; // 블록의 방향을 바꾸는 변수
int a,b,c,d; // 블록의 각 사각형을 가리키는 변수들
int wall[240]={0,}; // 블록에 벽이 있는지 없는지를 표시한다. 0이면 false, 1이면 true
int next_wall[240]={0,};
int ap,bp,cp,dp;
int next=0; // 다음 블록의 모양을 저장하는 변수
int next_rotation=0; // 다음 블럭의 방향을 저장하는 변수
int buttondrop=0; // 버튼이 눌렸는지를 확인하는 변수
int stop_button=0;
int score=0; // 점수를 저장하는 버튼
int height=227; // 가장 높은 블록
bool pos_flag=true; // position이 바뀌었는지 표시하는 변수
bool start_flag=false;
int waypos[18]={17,}; // 최적지점으로 움직이는 루트를 저장
int putpos;
int putro;
int waylen=0;
int wayro[18]={0,};

  void
_grid_create(UIData *ui);
  void
_tetris_delete(UIData *ui, int i);
void _draw_block(UIData* ui);
void _remove_block(UIData* ui);
void _make_shape();
void make_popup_end(UIData* ui);
  void
_win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
   UIData *ui = data;
   elm_win_lower(ui->win);
}

  bool
_check_flag() { // a,b,c,d가 모두 벽이 아니면 true를 리턴
   bool flag=true;

   if(wall[a]==1||wall[b]==1||wall[c]==1||wall[d]==1) {
      flag=false;
   }
   return flag;
}

  bool
  _is_edge(int i) { // 테투리인지 아닌지를 리턴
     bool flag=false;
     if(i/12==0) flag=true;
     if(i/12==19) flag=true;
     if(i%12==0) flag=true;
     if(i%12==11) flag=true;

     return flag;
  }

  bool
  _can_save(int i, int r) { // 저장될 수 있는 지점인지 (밑에가 바닥인지) 리턴
     bool flag=false;
     int prev_pos=pos;
     int prev_rotate=rotation;
     pos=i;
     rotation=r;
     _make_shape();
     if(_check_flag()) {
        pos+=12;
        _make_shape();
        if(_check_flag()) flag=false;
        else flag=true;
     } else flag=false;
     pos=prev_pos;
     rotation=prev_rotate;
     _make_shape();
     return flag;
  }

  void
_change_rotation() { // 블록의 종류에 따라, 방향을 변경해주는 함수
   if(shape==0||shape==3||shape==5) {
      if(rotation==0) rotation++;
      else rotation=0;
   }
   else if(shape==1||shape==2||shape==4) {
      if(rotation<3) rotation++;
      else rotation=0;
   }
}

  void
_make_shape() { // 블록의 모양, 방향에 따라 a,b,c,d 의 포지션을 정해준다

   switch(shape) {
   case(0) :
      if(rotation==0) {
         /*
          * ■
          * ■■
          *  ■
          */
         a=pos;
         b=pos+12;
         c=pos+25;
         d=pos+13;
      } else {
         /*
          *  ■■
          * ■■
          *
          * */
         a=pos+1;
         b=pos+12;
         c=pos+13;
         d=pos+2;
      }
      break;
   case(1):
      if(rotation==0) {
         /*
          * ■■
          * ■
          * ■
          * */
         a=pos;
         b=pos+12;
         c=pos+24;
         d=pos+1;
      } else if(rotation==1) {
         /*
          * ■
          * ■■■
          * */
         a=pos;
         b=pos+12;
         c=pos+13;
         d=pos+14;
      } else if(rotation==2) {
         /*
          *  ■
          *  ■
          * ■■
          * */
         a=pos+1;
         b=pos+24;
         c=pos+25;
         d=pos+13;
      } else if(rotation==3) {
         /*
          * ■■■
          *   ■
          *
          * */
         a=pos+1;
         b=pos;
         c=pos+14;
         d=pos+2;
      }
      break;
   case(2):
      if(rotation==0) {
         /*
          *  ■■
          *   ■
          *   ■
          * */
         a=pos+1;
         b=pos;
         c=pos+25;
         d=pos+13;
      } else if(rotation==1) {
         /*
          * ■■■
          * ■
          * */
         a=pos+1;
         b=pos;
         c=pos+12;
         d=pos+2;
      } else if(rotation==2) {
         /*
          * ■
          * ■
          * ■■
          * */
         a=pos;
         b=pos+12;
         c=pos+24;
         d=pos+25;
      } else if(rotation==3) {
         /*
          *   ■
          * ■■■
          * */
         a=pos+2;
         b=pos+12;
         c=pos+13;
         d=pos+14;
      }
      break;
   case(3):
      if(rotation==0) {
         /*
          *  ■
          * ■■
          * ■
          * */
         a=pos+1;
         b=pos+12;
         c=pos+24;
         d=pos+13;
      } else if(rotation==1) {
         /*
          * ■■
          *  ■■
          *
          * */
         a=pos+1;
         b=pos;
         c=pos+13;
         d=pos+14;
      }
      break;
   case(4):
      if(rotation==0) {
         /*
          *  ■
          * ■■■
          * */

         a=pos+1;
         b=pos+12;
         c=pos+13;
         d=pos+14;
      } else if(rotation==1) {
         /*  ■
          * ■■
          *  ■
          * */
         a=pos+1;
         b=pos+12;
         c=pos+13;
         d=pos+25;
      } else if(rotation==2) {
         /*
          *  ■■■
          *   ■
          * */
         a=pos+13;
         b=pos+12;
         c=pos+14;
         d=pos+25;
      } else if(rotation==3) {
         /*
          *  ■
          *  ■■
          *  ■
          * */
         a=pos+1;
         b=pos+13;
         c=pos+25;
         d=pos+14;
      }
      break;
   case(5):
      if(rotation==0) {
         /*
          *  ■
          *  ■
          *  ■
          *  ■
          * */
         a=pos+1;
         b=pos+13;
         c=pos+37;
         d=pos+25;
      } else if(rotation==1) {
         /*
          *  ■■■■
          *
          * */
         a=pos+1;
         b=pos;
         c=pos+2;
         d=pos+3;
      }
      break;
   case(6):
         /*
          * ■■
          * ■■
          * */
      a=pos+1;
      b=pos;
      c=pos+12;
      d=pos+13;
      break;
   }
}

 void
_draw_block(UIData *ui){ // 그려지게 될 자리에 벽이 없으면, 블록을 그려준다.

   int prev_a=a;
   int prev_b=b;
   int prev_c=c;
   int prev_d=d;
   int prev_rotation = rotation; // 새로 그릴 수 없으면 원래의 모양을 다시 그려야한다.

   _make_shape();
   if(_check_flag()) {
      evas_object_color_set(ui->rect[a],100,100,100,255);
      evas_object_color_set(ui->rect[b],100,100,100,255);
      evas_object_color_set(ui->rect[c],100,100,100,255);
      evas_object_color_set(ui->rect[d],100,100,100,255);
      pos_flag= true;
   } else { // 이 함수가 호출되기 전에 이미 remove한 상태이므로, 원래의 모양을 다시 그려야한다.
      a=prev_a;
      b=prev_b;
      c=prev_c;
      d=prev_d;
      rotation = prev_rotation;
      evas_object_color_set(ui->rect[a],100,100,100,255);
      evas_object_color_set(ui->rect[b],100,100,100,255);
      evas_object_color_set(ui->rect[c],100,100,100,255);
      evas_object_color_set(ui->rect[d],100,100,100,255);
      pos_flag=false;
   }

}

  void
_remove_block(UIData *ui){ // 블록을 옮기기 전에 지운다.

   evas_object_color_set(ui->rect[a],246,246,246,255);
   evas_object_color_set(ui->rect[b],246,246,246,255);
   evas_object_color_set(ui->rect[c],246,246,246,255);
   evas_object_color_set(ui->rect[d],246,246,246,255);

}


  GLboolean
_check_overlap(int i, int blocks[], int length) { // blocks[]에 이미 있는 블락인지 아닌지 체크한다
   for(int j=0; j<length; j++) {
      if(i==blocks[j]) return false;
      else return true;
   }
}

  float
_calc_value() { // 각 포지션(지금의 pos값)에 대한 점수를 매긴다. 주위에 블락이 많을수록, 테두리와 붙을수록, 높이가 낮을수록 높은 점수를 얻는다.
   int i =0;
   float value =0;
   int blocks[20]={0,}; // 그려진 블록의 주변 블록들과 abcd 블록을 저장할 것이다
   int length=4; // 지금 blocks 에는 a,b,c,d의 네개 블록이 있다.

   blocks[0]=a;
   blocks[1]=b;
   blocks[2]=c;
   blocks[3]=d;

   for(int j=0;j<4;j++) {
      // a,b,c,d 모두의 근처에 있는 블락들을 체크한다. 주변 블락들이 wall이거나, 테두리이면 해당 블록의 점수가 올라간다.
      i=blocks[j]-1;
      if(_check_overlap(i,blocks,length)&&wall[i]) {
         value+=2;
         if(_is_edge(i)&&(i/12)>(height/12)) value++;
         blocks[length++]=i;
      }
      i=blocks[j]+1;
      if(_check_overlap(i,blocks,length)&&wall[i]) {
         value+=2;
         if(_is_edge(i)&&(i/12)>(height/12)) value++;
         blocks[length++]=i;
      }
      i=blocks[j]-12;
      if(_check_overlap(i,blocks,length)&&wall[i]) {
         value+=2;
         if(_is_edge(i)&&(i/12)>(height/12)) value++;
         blocks[length++]=i;
      }
      i=blocks[j]+12;
      if(_check_overlap(i,blocks,length)&&wall[i]) {
         value+=2;
         if(_is_edge(i)&&(i/12)>(height/12)) value++;
         blocks[length++]=i;
      }

   }

   value+=(c/12)*1.5-10; // 높이에 따른 가산점수를 준다.

   for(i=0;i<20;i++) // blocks를 초기화한다.
      blocks[i]=0;
   if(pos==17) value=-999;
   return value;

}



  void
_put_block(UIData *data) { // value를 계산해서 최적값을 찾아 블록을 그린다.
   float value=0;
   float temp=0;
   int result_pos=0;
   int result_rotate=0;
   bool flag=true;

   for(int j=0;j<4;j++) {
      _change_rotation();
      for(int i=0;i<12;i++) {
         flag=true;
         for(int k=0;k<20;k++) {
            pos=i+(k*12);
            _make_shape();
            if(_check_flag()&&_can_save(pos,rotation)) {
               flag=false;
               temp=_calc_value();
               if(temp>value) {
                  result_pos=pos;
                  result_rotate=rotation;
                  value=temp;
               }
            }
            if(!flag) break;
         }
      }
   }

   putpos=result_pos;
   putro=result_rotate;

   waylen=(result_pos/12)-1;
   if(waylen==0)
      make_popup_end(data);
   pos=result_pos%12+12;
   rotation=result_rotate;
   _make_shape();

   _draw_block(data);
}

  void
next_block_draw(UIData *ui) { // 다음 블록을 그려주는 함수 - 우측 상단
   int temp[4]; // next block의 a,b,c,d 역할을 한다.
   int i,j;
   for(i=0;i<12;i++) // 전의 블락을 지운다
      evas_object_color_set(ui->next_rect[i],246,246,246,255);

   switch(next) { // 모양에 따라 temp 값을 지정한다.
            case(0) :
            temp[0]=0;
            temp[1]=3;
            temp[2]=4;
            temp[3]=7;
               break;
            case(1):
            temp[0]=0;
            temp[1]=1;
            temp[2]=3;
            temp[3]=6;
               break;
            case(2):
            temp[0]=0;
            temp[1]=1;
            temp[2]=4;
            temp[3]=7;
               break;
            case(3):
            temp[0]=2;
            temp[1]=4;
            temp[2]=5;
            temp[3]=7;
               break;
            case(4):
            temp[0]=4;
            temp[1]=6;
            temp[2]=7;
            temp[3]=8;
            break;
            case(5):
            temp[0]=1;
            temp[1]=4;
            temp[2]=7;
            temp[3]=10;
               break;
            case(6):
            temp[0]=6;
            temp[1]=7;
            temp[2]=9;
            temp[3]=10;
               break;
            }

         for(i=0;i<4;i++) { // 테이블에 그려준다.
            j=temp[i];
            evas_object_color_set(ui->next_rect[j],100,100,100,255);
         }
}

//블록 삭제된 후 다시 그리기(색칠)
  void
_draw_after(UIData *ui){
   int i=1;
   int j=1;
   for(i=1;i<19;i++)
      for(j=1;j<11;j++)
      {
        if(wall[i*12+j]==1){
            evas_object_color_set(ui->rect[i*12+j],100,100,100, 255);
        } else { //블록 존재 안 하면 배경색
            evas_object_color_set(ui->rect[i*12+j], 246,246,246, 255);
        }

      }

}

//블록 한 줄 다 채워지면 삭제(wall[])
  void
_tetris_delete(UIData *ui, int i){
   int k;
   for(k=1;k<11;k++) wall[i*12+k]=0; // i번째 줄은 다 찬 줄이므로, 지워준다 - 더이상 wall이 아니다

   //지워진 줄 위에 있는 줄을 밑으로 한 줄씩 내리기
   while(i>1) {
      for(k=1;k<11;k++) {
        wall[i*12+k]=wall[(i-1)*12+k];
        wall[(i-1)*12+k]=0;
      }
      i--;
   }
   _draw_after(ui);
}

// P334~ Touch event
/* Touch 다운 이벤트*/

  void
on_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) {

   UIData *ui=data;
   int temp_pos=pos;
   int temp_rot=rotation;
      if(stop_button!=1){
      // temppos : 0~270로, 사용자가 누른 rect객체의 ID.
      int temppos = atoi(evas_object_name_get(obj));

      // x, y : 사용자가 누른 Rect의 좌표값
      int x = temppos%12;
      int y = temppos/12;

      _remove_block(ui);
      // 블록의 아래쪽 방향을 누르면 아래로 두줄 이동한다.
      if((y-2>(pos/12))&&((x==a%12)||(x==b%12)||(x==c%12)||(x==d%12))){

         // 한 줄 이동
         pos+=12;
         _draw_block(ui);
         if(pos_flag) {
           pos+=12;
           _remove_block(ui);
           _draw_block(ui);
      }
      else pos-=12;
      }
      // 블록을 누르면 블록 모양을 바꾼다.
      else if(temppos==a||temppos==b||temppos==c||temppos==d){
         _change_rotation();
      }

      // 블록의 왼쪽 방향을 누르면 왼쪽으로 이동한다.
      else if(x<=(pos%12)){
         pos--;
      }

      // 블록의 오른쪽 방향을 누르면 오른쪽으로 이동한다.
      else if(x>(pos%12)){
         pos++;
      }

      _draw_block(ui);
      if(!pos_flag) {
         pos=temp_pos;
         rotation=temp_rot;
      }

   }
}



  void
stop_button_event(){
   if(stop_button==1){
      stop_button=0;
   }
   else{
    stop_button=1;
   }
}


//stop_button click
  void
continue_button_click(void *data,Evas_Object *obj, void *event_info){
   UIData *ui = data;
   stop_button_event();
   evas_object_del(ui->popup); //팝업창 삭제

}

  void
btn_reset_cb(void *data, Evas_Object *obj, void *event_info){
   stop_button_event();
   char buf[64];
   UIData *ui = data;
   _grid_create(ui);
   elm_object_content_set(ui->conform, ui->grid);
   score=0;
   sprintf(buf,"<font=Sans:style=Regular font_size=50>Score : %d</font/>",score);
   elm_object_text_set(ui->score_label, buf);
   evas_object_del(obj); //팝업창 삭제
}

  void
terminate_button_click(void *data, Evas_Object *obj, void *event_info){
    UIData *ui = data;
    ui=0;
    evas_object_del(ui->win);
}


// 게임이 종료되었을 경우 뜨는 팝업
  void
make_popup_end(UIData *u){
   Evas_Object *btn;
   UIData *ui = u;

   //우선 블록을 멈춤
   stop_button_event();

   //죽은것을 표시 (위에까지 색칠)
   evas_object_color_set(ui->rect[a],100,100,100,255);
   evas_object_color_set(ui->rect[b],100,100,100,255);
   evas_object_color_set(ui->rect[c],100,100,100,255);
   evas_object_color_set(ui->rect[d],100,100,100,255);

   //popup 생성
   ui->popup=elm_popup_add(ui->grid); //팝업 생성
   elm_popup_align_set(ui->popup,ELM_NOTIFY_ALIGN_FILL,1.0); //위치 설정
   evas_object_size_hint_weight_set(ui->popup,EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   //다시하기 버튼 삽입
   btn=elm_button_add(ui->popup);
   elm_object_text_set(btn,"다시하기");
   elm_object_part_content_set(ui->popup,"button1",btn);
   evas_object_smart_callback_add(btn,"clicked",btn_reset_cb,ui);
   evas_object_show(btn);

   //그만하기 버튼 삽입
   btn=elm_button_add(ui->popup);
   elm_object_text_set(btn,"그만하기");
   elm_object_part_content_set(ui->popup,"button2",btn);
   evas_object_smart_callback_add(btn,"clicked",terminate_button_click,ui);
   evas_object_show(btn);

   evas_object_show(ui->popup);
}

// 메뉴바의 POPUP 버튼 클릭한 경우 이벤트
  void
make_popup_button(void *data, Evas_Object *obj, void *event_info){
   Evas_Object *btn;
   UIData *ui = data;

   //우선 블록을 멈춤
   stop_button_event();

   //popup 생성
   ui->popup=elm_popup_add(ui->grid); //팝업 생성
   elm_popup_align_set(ui->popup,ELM_NOTIFY_ALIGN_FILL,1.0); //위치 설정
   evas_object_size_hint_weight_set(ui->popup,EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   //계속하기 버튼 삽입
   btn=elm_button_add(ui->popup);
   elm_object_text_set(btn,"계속하기");
   elm_object_part_content_set(ui->popup,"button1",btn);
   evas_object_smart_callback_add(btn,"clicked",continue_button_click,ui);
   evas_object_show(btn);

   //다시하기 버튼 삽입
   btn=elm_button_add(ui->popup);
   elm_object_text_set(btn,"다시하기");
   elm_object_part_content_set(ui->popup,"button2",btn);
   evas_object_smart_callback_add(btn,"clicked",btn_reset_cb,ui);
   evas_object_show(btn);

   //그만하기 버튼 삽입
   btn=elm_button_add(ui->popup);
   elm_object_text_set(btn,"그만하기");
   elm_object_part_content_set(ui->popup,"button3",btn);
   evas_object_smart_callback_add(btn,"clicked",terminate_button_click,ui);
   evas_object_show(btn);

   evas_object_show(ui->popup);
}
  //블록 저장
   void
 _save_block(UIData *ui) {
       int i=0,j=0;
       char buf[64]; // 점수를 입력하기 위한 buffer
       bool flag=true; // 줄이 채워져있나를 검사하는 플래그

       // 지금 위치에 블록을 저장
       wall[a]=1;
       wall[b]=1;
       wall[c]=1;
       wall[d]=1;

       if(a<height) height=a; // 블록들이 저장된 높이를 업데이트 한다.
       if((a/12)==1) make_popup_end(ui);
       for(i=1;i<19;i++) {
          flag=true;
          for(j=1;j<11;j++)
             if(wall[i*12+j]==0) flag=false;
          if(flag) // 모든 사각형이 차있다면 한 줄을 지워서 새로 그리고 점수를 올린다.
          {
            _tetris_delete(ui,i);
            score++;
            sprintf(buf,"<font=Sans:style=Regular font_size=50>Score : %d</font/>",score);
            elm_object_text_set(ui->score_label, buf);
          }
       }

       // 새로운 블락을 그린다
       pos=17;
       shape=next;
       rotation = next_rotation;
       next=rand()%7;
       // next block을 바꿔줌
       next_block_draw(ui);
       rotation=0;
       //죽었는지 검사
       if(_can_save(pos,rotation)||!_check_flag()) {
           make_popup_end(ui);

       }

       waylen=0;

    }

  void
_init_draw_block(UIData *ui, int i){ // 처음에 테트리스 판의 wall 값을 초기화하고 첫 블록을 그리는 함수
   int j=0;
   pos = i;
   //_draw_block(ui);

   for(j=0;j<240;j++) {
      if(_is_edge(j)) wall[j]=1;
      else {
         wall[j]=0;
      }

   }

}

  void
_rect_table_create(UIData *ui) {
   int i=0;
   int x=0,y=0;

   //Rect 테이블 생성
   ui->rect_table = elm_table_add(ui->grid);

   // rect_table : 12x20
   for(i=0;i<240;i++) {

     ui->rect[i]=evas_object_rectangle_add(ui->rect_table);
     evas_object_color_set(ui->rect[i], 246,246,246, 255);

      // 테두리
     if(i%12==0||i%12==11||i/12==0||i/12==19)
         evas_object_color_set(ui->rect[i],6,162,203,255);

     evas_object_size_hint_min_set(ui->rect[i], 50, 50);

      y=i/12;
      x=i%12;

     elm_table_padding_set(ui->rect_table,5,5);
     elm_table_pack(ui->rect_table, ui->rect[i],x,y,1,1);

     // 클릭된 사각형의 x좌표 등록
     char *num[3];
     sprintf(num, "%d", i);
     evas_object_name_set(ui->rect[i], num);

     //사각형별 콜백함수
     evas_object_event_callback_add(ui->rect[i],   EVAS_CALLBACK_MOUSE_DOWN, on_mouse_down, ui);

     evas_object_show(ui->rect[i]);
   }

}

  void
_next_rect_table_create(UIData *ui) {
   int i=0;
   int x,y;
   ui->next_block_table = elm_table_add(ui->grid);

   for(i=0;i<12;i++) { // next_rect[] 색칠
      ui->next_rect[i]=evas_object_rectangle_add(ui->next_block_table);
      evas_object_color_set(ui->next_rect[i],246,246,246, 255);
      evas_object_size_hint_min_set(ui->next_rect[i],25,25);

      y=i/3;
      x=i%3;

      // next_block_table에 사각형 패킹
      elm_table_padding_set(ui->next_block_table, 5,5);
      elm_table_pack(ui->next_block_table,ui->next_rect[i],x,y,1,1);
      evas_object_show(ui->next_rect[i]);
   }
}
  void
_grid_create(UIData *ui)
{

   Evas_Object *btn;

   //Grid 생성
   ui->grid = elm_grid_add(ui->conform);
   evas_object_show(ui->grid);

   //팝업창 버튼
   btn=elm_button_add(ui->conform);
   elm_object_text_set(btn,"POPUP");
   evas_object_smart_callback_add(btn,"clicked",make_popup_button, ui);
   elm_grid_pack(ui->grid,btn, 10, 1, 25, 3);
   evas_object_show(btn);

   // 점수 라벨 등록
   char buf[64];
   ui->score_label=elm_label_add(ui->grid);
   sprintf(buf,"<font=Sans:style=Regular font_size=50>Score : %d</font/>",score);
   elm_object_text_set(ui->score_label, buf);
   elm_grid_pack(ui->grid,ui->score_label,10,5,50,10);
   evas_object_show(ui->score_label);

   // rect_table 생성
   _rect_table_create(ui);

   /* wall[] 값 초기화 & 최초 블록 생성*/
   _init_draw_block(ui,17);

   // next block table 등록
   _next_rect_table_create(ui);

   // 다음 블록 값 생성
   next=rand()%7;
   next_block_draw(ui);

   // grid 에 두 테이블 패킹
   elm_grid_pack(ui->grid,ui->rect_table,5,15,90,80);
   elm_grid_pack(ui->grid,ui->next_block_table, 85,1,10,10);
   evas_object_show(ui->rect_table);
   evas_object_show(ui->next_block_table);

}


  Eina_Bool timer_func(void *data) {

      UIData *ui=data;

     if(!stop_button){
        if(waylen==0){
           _put_block(ui);
        } else {
          if(_can_save(pos,rotation)) {
             _save_block(ui);
          } else {
                _remove_block(ui);
                pos+=12;
                rotation=putro;
                _make_shape();
                _draw_block(ui);
           }
        }
      }
  }


  void
_conformant_create(UIData *ui)
{
   ui->conform = elm_conformant_add(ui->win);

   //Specify to conformant occupies all allocated to him space
  // (horizontally and vertically)
   evas_object_size_hint_weight_set(ui->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   // Create a layout and place it into a conformant
   _grid_create(ui);
   elm_object_content_set(ui->conform, ui->grid);

   evas_object_show(ui->conform);
}

  void
_window_create(UIData *ui)
{
   // Create and configure the main window
   ui->win = elm_win_util_standard_add(NULL, NULL);

       // Create a conformant - in this case, the main container in the application,
       // which is also the interface between the application and
       // system elements, such as a keyboard.
   _conformant_create(ui);

   //  Set the size of the conformant widget the same as the size of the window
   elm_win_resize_object_add(ui->win, ui->conform);

    //Register the function that will be called
    //when you press the hardware Back button
   eext_object_event_callback_add(ui->win, EEXT_CALLBACK_BACK, _win_back_cb, ui);

   // /Always display the window only after the entire base
   // user interface will be displayed.
   evas_object_show(ui->win);
   start_flag=true;

}

  bool
_app_create_cb(void *data)
{
   UIData *ui = data;

   timer1=ecore_timer_add(0.01,timer_func,ui);

   return true;
}

void
create_game_start_cb(UIData *ui)
{
   _window_create(ui);
   _app_create_cb(ui);

}
