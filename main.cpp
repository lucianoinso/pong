#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
const int pad_SIZE_W = 8;
const int pad_SIZE_H = 60;
const int p1_pad_SPEED = 6;
const int p2_pad_SPEED = 6;
const int ball_SIZE = 8;
const int ball_SPEED = 6;
const float ball_init_x = SCREEN_W / 2.0 - ball_SIZE / 2.0;
const float ball_init_y = SCREEN_W / 2.0 - ball_SIZE / 2.0;

enum MYKEYS { KEY_UP, KEY_DOWN };

int main(int argc, char **argv)
{
   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;
   ALLEGRO_TIMER *timer = NULL;
   ALLEGRO_BITMAP *p1_pad = NULL, *p2_pad = NULL;
   ALLEGRO_BITMAP *ball = NULL;
   ALLEGRO_SAMPLE *pad1_sfx=NULL;
   ALLEGRO_SAMPLE *pad2_sfx=NULL;
   ALLEGRO_SAMPLE *flawless_sfx=NULL;
   ALLEGRO_SAMPLE *victory_sfx=NULL;

   srand (time(NULL));
   int p1_pad_points = 0, p2_pad_points = 0;
   int ia_choice = 0;

   float p1_pad_x = pad_SIZE_W;
   float p1_pad_y = SCREEN_H/2 - pad_SIZE_H/2;
   
   float p2_pad_x = SCREEN_W - (pad_SIZE_W)*2;
   float p2_pad_y = SCREEN_H/2 - pad_SIZE_H/2;

   float ball_x = ball_init_x;
   float ball_y = ball_init_y;
   float ball_dx = -ball_SPEED, ball_dy = ball_SPEED;

   int difficulty_ratio = 20;

   bool key[2] = { false, false};
   bool redraw = true;
   bool doexit = false;

   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }
   
   al_init_font_addon();
   al_init_ttf_addon();

   if(!al_install_keyboard()) {
      fprintf(stderr, "failed to initialize the keyboard!\n");
      return -1;
   }

   if(!al_install_audio()){
      fprintf(stderr, "failed to initialize audio!\n");
      return -1;
   }
 
   if(!al_init_acodec_addon()){
      fprintf(stderr, "failed to initialize audio codecs!\n");
      return -1;
   }
 
   if (!al_reserve_samples(1)){
      fprintf(stderr, "failed to reserve samples!\n");
      return -1;
   }

   pad1_sfx = al_load_sample("p1.wav");
   if (!pad1_sfx){
      printf( "Audio clip sample not loaded!\n" ); 
      return -1;
   }

   pad2_sfx = al_load_sample("p2.wav");
   if (!pad2_sfx){
      printf( "Audio clip sample not loaded!\n" ); 
      return -1;
   }
   
   flawless_sfx = al_load_sample("fv.wav");
   if (!flawless_sfx){
      printf( "Audio clip sample not loaded!\n" ); 
      return -1;
   }
   
   char* sfx_array[] = {"vsfx1.wav", "vsfx2.wav", "vsfx3.wav", "vsfx4.wav", "vsfx5.wav"};
   int sfx_choice = rand() % 5;
   
   victory_sfx = al_load_sample(sfx_array[sfx_choice]);
   if (!victory_sfx){
	   printf( "Audio clip sample not loaded!\n" ); 
      return -1;
   }

   timer = al_create_timer(1.0 / FPS);
   if(!timer) {
      fprintf(stderr, "failed to create timer!\n");
      return -1;
   }
 
   display = al_create_display(SCREEN_W, SCREEN_H);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      al_destroy_timer(timer);
      return -1;
   }
 
   p1_pad = al_create_bitmap(pad_SIZE_W, pad_SIZE_H);
   if(!p1_pad) {
      fprintf(stderr, "failed to create p1_pad bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   al_set_target_bitmap(p1_pad);
   al_clear_to_color(al_map_rgb(150, 0, 200));
   
   p2_pad = al_create_bitmap(pad_SIZE_W, pad_SIZE_H);
   if(!p2_pad) {
	   fprintf(stderr, "failed to create p2_pad bitmap!\n");
	   al_destroy_display(display);
	   al_destroy_timer(timer);
	   al_destroy_bitmap(p1_pad);
	   return -1;
   }
   al_set_target_bitmap(p2_pad);
   al_clear_to_color(al_map_rgb(200, 0, 0));

   ALLEGRO_FONT *gameoverfont = al_load_ttf_font("pirulen.ttf",32,0 );
   if (!gameoverfont){
      fprintf(stderr, "Could not load 'pirulen.ttf'.\n");
      return -1;
   }
   ALLEGRO_FONT *pointsfont = al_load_ttf_font("sansserif.ttf",16,0);
   if (!pointsfont){
	   fprintf(stderr, "Could not load 'sansserif.ttf'.\n");
	   return -1;
   }
   
   ball = al_create_bitmap(ball_SIZE, ball_SIZE);
   if(!ball){
	  fprintf(stderr, "failed to create ball bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   al_set_target_bitmap(ball);
   al_clear_to_color(al_map_rgb(150,10,100));

   al_set_target_bitmap(al_get_backbuffer(display));
 
   event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_bitmap(p1_pad);
      al_destroy_bitmap(ball);
	  al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
 
   al_register_event_source(event_queue, al_get_display_event_source(display));
 
   al_register_event_source(event_queue, al_get_timer_event_source(timer));
 
   al_register_event_source(event_queue, al_get_keyboard_event_source());
 
   al_clear_to_color(al_map_rgb(0,0,0));
 
   al_flip_display();
 
   al_start_timer(timer);
 
   while(!doexit)
   {
      ALLEGRO_EVENT ev;
	  al_wait_for_event(event_queue, &ev);
 
      if(ev.type == ALLEGRO_EVENT_TIMER) {
	
		 if((((ball_y >= p1_pad_y) && (ball_y <= (p1_pad_y + pad_SIZE_H))) && (ball_x == p1_pad_x + pad_SIZE_W))) {
            ball_dx = -ball_dx;
			al_play_sample(pad1_sfx, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
         }

		 if((((ball_y >= p2_pad_y) && (ball_y <= (p2_pad_y + pad_SIZE_H))) && (ball_x == p2_pad_x - pad_SIZE_W))) {
            ball_dx = -ball_dx;
			al_play_sample(pad2_sfx, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
         }
 
         if(ball_y < 0 || ball_y > SCREEN_H - ball_SIZE) {
            ball_dy = -ball_dy;
		 }

		 if((ball_x >= SCREEN_W) || (ball_x<=0)){
			if(ball_x <= 0){
				p2_pad_points++;
				ball_x = ball_init_x;
				ball_y = ball_init_y;
				ball_dx = ball_SPEED;
				ball_dy = ball_SPEED;
			}
			else{
				p1_pad_points++;
				ball_x = ball_init_x;
				ball_y = ball_init_y;
				ball_dx = -ball_SPEED;
				ball_dy = ball_SPEED;
			}
		 }

		 ball_x += ball_dx;
		 ball_y += ball_dy;

		 ia_choice = rand() % difficulty_ratio;

		 if(ball_y <= (p2_pad_y + (pad_SIZE_H/2)))
		 {
			if (ia_choice > 1 && p2_pad_y>0){
				p2_pad_y-=p2_pad_SPEED;
			}
		 }

		 if(ball_y >= (p2_pad_y + (pad_SIZE_H/2)))
		 {
			 if (ia_choice > 1 && (p2_pad_y < SCREEN_H - pad_SIZE_H)){
				p2_pad_y+=p2_pad_SPEED;
			}
		 }

      if(key[KEY_UP] && p1_pad_y >= p1_pad_SPEED) {
         p1_pad_y -= p1_pad_SPEED;
      }
 
      if(key[KEY_DOWN] && p1_pad_y <= SCREEN_H - pad_SIZE_H - p1_pad_SPEED) {
         p1_pad_y += p1_pad_SPEED;
	  }

      redraw = true;
   
	  }
      else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
         break;
      }
      else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
         switch(ev.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
               key[KEY_UP] = true;
               break;
 
            case ALLEGRO_KEY_DOWN:
               key[KEY_DOWN] = true;
               break;
      }
   }
   else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
      switch(ev.keyboard.keycode) {
         case ALLEGRO_KEY_UP:
            key[KEY_UP] = false;
            break;
 
         case ALLEGRO_KEY_DOWN:
            key[KEY_DOWN] = false;
               break;
   		 case ALLEGRO_KEY_1:
			   difficulty_ratio = 10;
			   break;
		 case ALLEGRO_KEY_2:
			   difficulty_ratio = 20;
			   break;
		 case ALLEGRO_KEY_3:
			   difficulty_ratio = 40;
			   break;

         case ALLEGRO_KEY_ESCAPE:
            doexit = true;
            break;
      }
   }
 
   if(redraw && al_is_event_queue_empty(event_queue)) {
	     redraw = false;
         al_clear_to_color(al_map_rgb(0,0,0));
         al_draw_bitmap(p1_pad, p1_pad_x, p1_pad_y, 0);
		 al_draw_bitmap(p2_pad, p2_pad_x, p2_pad_y, 0);
		 al_draw_bitmap(ball, ball_x, ball_y, 0);
		 al_draw_textf(pointsfont, al_map_rgb(100,210,100), 0, 4, ALLEGRO_ALIGN_LEFT,
				"P1 Points: %i", p1_pad_points);
		 al_draw_textf(pointsfont, al_map_rgb(100,210,100), 640, 4, ALLEGRO_ALIGN_RIGHT,
				"P2 Points: %i", p2_pad_points);
		 al_draw_textf(pointsfont, al_map_rgb(100,210,100), 640, 24, ALLEGRO_ALIGN_RIGHT,
				"P2 I.A choice: %i", ia_choice);
		 
		 if(p1_pad_points == 10){
			al_draw_text(gameoverfont, al_map_rgb(200,200,25), 640/2, 480/4,ALLEGRO_ALIGN_CENTRE,
				"Player 1 Wins");
			al_play_sample(victory_sfx, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			al_flip_display();
			al_rest(7);

			if(p2_pad_points == 0){
				al_draw_text(gameoverfont, al_map_rgb(200,200,25), 640/2, 480/4 + 30,ALLEGRO_ALIGN_CENTRE,
				"Flawless Victory");
				al_play_sample(flawless_sfx, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			}
			
			al_flip_display();
			doexit=true;
			al_rest(5);
		 }

		 if(p2_pad_points == 10){
			al_draw_text(gameoverfont, al_map_rgb(200,200,25), 640/2, 480/4,ALLEGRO_ALIGN_CENTRE,
				"Player 2 Wins");
			al_play_sample(victory_sfx, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			al_flip_display();
			al_rest(7);

			if(p1_pad_points == 0){
				al_draw_text(gameoverfont, al_map_rgb(200,200,25), 640/2, 480/4 + 30,ALLEGRO_ALIGN_CENTRE,
				"Flawless Victory");
				al_play_sample(flawless_sfx, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			}

			al_flip_display();
			doexit=true;
			al_rest(4);
		 }
		 al_flip_display();
      }
   }

   al_destroy_bitmap(p1_pad);
   al_destroy_bitmap(ball);
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_sample(pad1_sfx);
   al_destroy_sample(pad2_sfx);
   al_destroy_sample(flawless_sfx);
   al_destroy_event_queue(event_queue);
 
   return 0;
}