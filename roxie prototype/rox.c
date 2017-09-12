#include <SDL2/SDL.h>
#include <stdlib.h>
enum tile{
	EMPTY,
	SOLID,
	DESTRUCT,
};
enum player_state{
	STAND,
	RUNNING,
	DASHING,
	WALLCLIMB,
	BOOSTCLIMB,
	FALL,
	WALLJUMP,
};
struct point{
	unsigned x,y;
};
struct player{
	struct point pos;
	enum player_state state;
	char dir;
};
struct input{
	char dir;
	char vel;
};
struct input get_input(void);
void Frame(void);

typedef char tile;
#define WALK_TIME 2.0
#define RUNNING_TIME 1.0
#define PF_W 256
#define PF_H 256
#define FLICK 16
tile level[PF_W][PF_H]={
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};
static struct player player;
const int DIM = (FLICK*16);
static int w=DIM;
static int h=DIM;
static SDL_Window*win;
static SDL_Renderer*ren;
static int mouse_held;
void COLOR(int r, int g, int b){
	SDL_SetRenderDrawColor(ren, r, g, b, 0);
}
void RECT(int x, int y, int w, int h){
	SDL_Rect r={x,y,w,h};
	SDL_RenderFillRect(ren, &r);
}
void WRAP(unsigned *x, unsigned *y){
	*x%=PF_W;
	*y%=PF_H;
}
void WRAP_P(struct point *pos, int xo, int yo){
	pos->x+=xo;
	pos->y+=yo;
	WRAP(&pos->x, &pos->y);
}
void draw_scene(void){
	#define SIZ (DIM/FLICK)
	COLOR(0,0,0);
	SDL_RenderClear(ren);
	COLOR(255,255,255);
	
	for(int x=0;x<FLICK;x++)
		for(int y=0;y<FLICK;y++){
			struct point cam = player.pos;
			cam.x-=FLICK/2;
			cam.y-=FLICK/2;
			WRAP_P(&cam,x,y);
			if(level[cam.x][cam.y]!=EMPTY)
				RECT(x*SIZ,y*SIZ,SIZ,SIZ);
		}
	if( player.state == DASHING)
		COLOR(0,0,255);
	else
		COLOR(255,0,0);
	RECT((FLICK/2)*SIZ,(FLICK/2)*SIZ,SIZ,SIZ);
	SDL_RenderPresent(ren);
}

enum tile tile_at( struct point pos, int xo, int yo ){
	WRAP_P(&pos, xo, yo);
	return level[pos.x][pos.y];
}
void crush( struct point pos, int xo, int yo){
	WRAP_P(&pos, xo,yo);
	level[pos.x][pos.y]=EMPTY;
	SDL_Delay(100);
}
void move( struct player*player, int xo, int yo, float time){
	SDL_Delay(100*time);
	WRAP_P(&player->pos, xo, yo);
}

enum player_state handle_player( struct player *player, struct input input){
	switch(player->state){
	case STAND:
		if(input.dir!=0){
			player->dir = input.dir;
			if(input.vel==0){
				return RUNNING;
			}else{
				return DASHING;
			}
		}
	break;
	case RUNNING:
		if( tile_at( player->pos, 0, 1)==EMPTY){
			return FALL;
		}if( input.vel > 0){
			return DASHING;
		}else if( input.dir != player->dir){
			return STAND;
		}else if( tile_at( player->pos, player->dir, 0 ) != EMPTY ){
			return WALLCLIMB;
		}else{
			move( player, player->dir, 0, WALK_TIME );
		}
	break;
	case DASHING:
	{
		enum tile t = tile_at( player->pos, player->dir, 0 ); 
		if( t != EMPTY ){
			if( t == DESTRUCT){
				crush( player->pos, player->dir, 0);
				return STAND;
			}else{
				return WALLCLIMB;
			}
		}else if( input.vel == 0 ){
			if( tile_at( player->pos, 0, 1) == EMPTY){
				return FALL;
			}else{
				return RUNNING;
			}
		}else{
			move( player, player->dir, 0 , RUNNING_TIME);
		}
	break;
	}
	case WALLCLIMB:
		if( input.vel >0){
			//return BOOSTCLIMB;
		}
		if( tile_at( player->pos, player->dir, 0) == EMPTY ){
			move( player, player->dir, 0, WALK_TIME);
			return STAND;
		}else if( tile_at( player->pos, 0, -1 ) != EMPTY ){
			return FALL;
		}else if( input.dir == -player->dir ){
			player->dir = -player->dir;
			return WALLJUMP;
		}else if( input.dir == 0){
			return FALL;
		}else{
			move( player, 0, -1, WALK_TIME );
		}	
	break;
	case BOOSTCLIMB:
	if( tile_at( player->pos, 0, -1 ) != EMPTY ){
		return FALL;
	}else if(input.dir==0){
		return FALL;
	}else{
		move( player, 0, -1, RUNNING_TIME );
	}
	break;
	case FALL:
		switch(tile_at( player->pos, 0, 1 )){
			case EMPTY:
				move( player, 0, 1, RUNNING_TIME );
			break;
			case DESTRUCT:
				crush(player->pos, 0, 1);
			default:
			return STAND;

		}
	break;
	case WALLJUMP:
		if(tile_at( player->pos, player->dir, 0)!=EMPTY){
			return WALLCLIMB;
		}else if( input.dir == 0){
			return FALL;
		}else{
			move( player, player->dir, 0, WALK_TIME );
		}
	break;
	}
	return player->state;
}

void update_game(void){
	draw_scene();
	Frame();
	player.state = handle_player( &player, get_input());
}

void Quit(void){
	SDL_DestroyWindow(win);
	SDL_DestroyRenderer(ren);
	SDL_Quit();
	exit(0);
}
void Frame(void){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		if(e.type==SDL_QUIT){
			Quit();
		}else if(e.type==SDL_MOUSEBUTTONDOWN){
			mouse_held = 1;
		}else if(e.type==SDL_MOUSEBUTTONUP){
			mouse_held = 0;
		}
	}
}
struct input get_input(void){
	struct input input;
	int x, y;
	SDL_GetMouseState(&x,&y);
	if( mouse_held ){
		input.dir=(x>w/2)?1:-1;
		if( x < w/2 - w/4 || x> w/2 + w/4){
			input.vel = 1;
		}else{
			input.vel = 0;
		}
	}else{
		input.dir = 0;
		input.vel = 0;
	}
	return input;
}
int main(){
	SDL_Init(SDL_INIT_VIDEO);
	win=SDL_CreateWindow("Parkour",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,w,h,SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	player.pos.x=2;
	player.pos.y=2;
	player.dir=1;
	player.state=STAND;
	for(int i=0;i<16;i++){
		level[i][14]=SOLID;
	}
	for(;;){
		update_game();
	}
	return 0;
}
