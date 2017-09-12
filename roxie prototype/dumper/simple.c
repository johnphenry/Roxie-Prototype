#include <stdio.h>
#include <stdlib.h>
#include <tmx.h>
void*convert_map(tmx_map *map){
	unsigned dim = map->tile_width;
	if(!map)return;
	tmx_layer *layer= map->ly_head;
	for(;layer;layer=layer->next){
		switch(layer->type){
		case L_LAYER:
			if(layer->content.gids) for(int i=0; i<map->height * map->width; i++){
				printf("%d,", layer->content.gids[i] & TMX_FLIP_BITS_REMOVAL);
			}
		break;
		case L_OBJGR:{
			tmx_object*object = layer->content.objgr->head;
			for(;object;object=object->next){
				printf("\n%sX:%d Y:%d", object->type, (int)object->x/dim, (int)object->y/dim);
			}
		}
		break;
		default:
			continue;
		}
	}
}
int main( int argc, char*argv[]){
	if(argc<2)return 1;
	tmx_map*map=tmx_load(argv[1]);
	if(!map)return 1;
	convert_map(map);
	tmx_map_free(map);
	return 0;
}
