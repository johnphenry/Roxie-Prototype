enum tile{
	EMPTY,
	SOLID,
	DESTRUCT,
};
typedef char tile;
#define PF_W 256
#define PF_H 256
tile level[PF_W][PF_H];
int px,py;
int dir;
/*In fact, just run a linear interpolation between every single move for this prototype.
in this way, the animations will not even be programmed into the states themselves.

Also, if during the animation, one changes their mind, then they should be free to do so.
Allow some animations to "allow cancelling" with others.

Homestly, instead of an enum, an is_solid, or is_empty function may be useful for scalability purposes.
The prototype, however, does not really need this.
*/
void read_mouse(){

}

int main(){
	STAND:

	RUNNING:
		/* Animation: Linear interpolation between subpixels, using subpixels.*/
		if(tile_at(dir,0)!=EMPTY){
			goto WALLCLIMB;
		}
		move(dir,0);
		if(INTERPOLATING){
			ALLOW_TURNAROUND;
		}	
	FALL:
		if(tile_at(0,1)==EMPTY){
			py+=1;
		}else if(DESTRUCT){
			level[px][py]=EMPTY;
		}else{
			goto STAND;
		}
	WALLCLIMB:
		if(tile_at(0, -1)!=EMPTY)
			goto FALL;
		py-=1;
		if( tile_at( dir, 0 ) == EMPTY ){
			goto CORNER_HANDLE;
		}
		if(input_did_change){
			if(input_dir == -dir){
				goto WALLJUMP;
			}else if(input_vel == HEAVY_INPUT){
				goto RUN_CLIMB;
			}
		}
	CORNER_HANDLE:
		move(dir,0);
		goto STAND;
	DASH_UP:
		move(0,-1);
		if(input_vel == LIGHT_INPUT){
			goto WALLCLIMB;
		}
	DASH:
		if(input_vel == LIGHT_INPUT){
			if( in_air ){
				goto FALLING;
			}
			else{
				goto RUNNING;
			}
		}
		if(tile_at(dir,0)!=EMPTY){
			goto DASH_REBOUND;
		}else{
			move(1,0);
		}
	DASH_REBOUND:
		Play_animation();
		goto STAND;
	return 0;
}
