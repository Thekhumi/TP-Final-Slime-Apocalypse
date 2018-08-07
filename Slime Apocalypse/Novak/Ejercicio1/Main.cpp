#ifndef _DEBUG
#define ALLEGRO_STATICLINK
#endif // !_DEBUG
#define PLAYER_FILE "player.png"
#define ENEMY_FILE "enemy.png"
#define BULLET_FILE "bullet.png"
#define MENU_FILE "menu.png"
#define HIT_FILE "Hit.ogg"
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include "allegro5/allegro_audio.h"
#include "allegro5/allegro_acodec.h"
#include "Bala.h"
#include "Enemigo.h"
#include <ctime>
/*constantes*/
const float FPS = 60;		//tasa de fps
const int SCREEN_W = 640;	//ancho de pantalla
const int SCREEN_H = 480;	//largo de pantalla
const int PLAYER_SIZE = 31;	//tamaño de imagen de  jugador
const int BULLET_SIZE = 8;	//tamaño de imagen de la bala
const int ENEMY_SIZE = 64;	// tamaño de imagen de enemigo

enum LANES {

};
enum MYKEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE
};

bool colision(float player_x, float player_y, int PLAYER_SIZE, float enemy_x, float  enemy_y, int ENEMY_SIZE) {
	return ((player_x < enemy_x + ENEMY_SIZE) && (enemy_x < player_x + PLAYER_SIZE)
		&& (player_y < enemy_y + ENEMY_SIZE) && (enemy_y < player_y + PLAYER_SIZE));
}

int main(int argc, char **argv)
{
	srand(time(0));
	ALLEGRO_DISPLAY *display = NULL;					// pantalla
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;			//cola de eventos
	ALLEGRO_TIMER *timer = NULL;						//timer de juego
	ALLEGRO_BITMAP *player = NULL;						//bitmap de player
	ALLEGRO_BITMAP *menu = NULL;
	float player_x = SCREEN_W / 2.0 - PLAYER_SIZE / 2.0;//posicion en x de player
	float player_y = SCREEN_H / 2.0 - PLAYER_SIZE / 2.0;//posicion en y de player
	Enemigo* enemigos[5];
	ALLEGRO_BITMAP * enBitmaps[5];						// bitmap de enemigo
	for (int i = 0; i < 5; i++)
	{
		int randPosX = rand() % 500 + 250;
		int randPosY = rand() % 100 + 100;
		enemigos[i] = new Enemigo(randPosX,randPosY);
		enBitmaps[i] = NULL;
	}
	int vidas = 3;
	int shootTimer = 0;
	bool shooted = false;
	bool redraw = true;
	bool doexit = false;
	bool key[5] = { false, false, false, false, false };		//arreglo de teclas
	Bala* balas[5];
	ALLEGRO_BITMAP * bitmaps[5];
	/* Inicializar Balas */
	for (int i = 0; i < 5; i++)
	{
		balas[i] = new Bala();
		bitmaps[i] = NULL;
	}
	/*inicializo allegro*/
	if (!al_init()) {
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	/*inicializo addon para cargar imagenes*/
	if (!al_init_image_addon()) {
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}
	al_init_acodec_addon();
	/*inicializo el timer del juego*/
	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize timer!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	/*inicializo la ventana*/
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize display!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_timer(timer);
		return -1;
	}
	/*inicializo teclado*/
	if (!al_install_keyboard()) {
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize keyboard!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	al_install_audio();
	/*cargo la imagen del player*/
	player = al_load_bitmap(PLAYER_FILE);
	if (!player) {
		al_show_native_message_box(display, "Error", "Error", "Failed to load bouncer!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
	/*cargo la imagen del enemigo*/
	for (int i = 0; i < 5; i++){
		enBitmaps[i] = al_load_bitmap(ENEMY_FILE);
	}
	menu = al_load_bitmap(MENU_FILE);
	ALLEGRO_SAMPLE *hit = al_load_sample(HIT_FILE);
	al_reserve_samples(1);
	for (int i = 0; i < 5; i++) {
		bitmaps[i] = al_load_bitmap(BULLET_FILE);
	}
	al_clear_to_color(al_map_rgb(255, 0, 255));

	al_set_target_bitmap(al_get_backbuffer(display));

	/*inicializo cola de eventos*/
	event_queue = al_create_event_queue();
	if (!event_queue) {
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize event queue!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
	}

	al_register_event_source(event_queue, al_get_display_event_source(display)); //inicializo eventos de ventana

	al_register_event_source(event_queue, al_get_timer_event_source(timer)); //inicializo eventos de timer

	al_register_event_source(event_queue, al_get_keyboard_event_source()); //inicializo eventos de teclado

	al_clear_to_color(al_map_rgb(0, 0, 0)); //pongo pantalla en negro

	al_flip_display();
	al_draw_bitmap(menu, 1, 1, 0);
	al_flip_display();
	bool start = false;
	while (!start){
		ALLEGRO_EVENT evMenu;
		al_wait_for_event(event_queue, &evMenu);
		if (evMenu.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (evMenu.keyboard.keycode == ALLEGRO_KEY_SPACE || evMenu.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				start = true;
			}
		}
	}

	al_start_timer(timer); //inicio el timer
	while (!doexit)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			if (key[KEY_UP] && player_y >= 4.0) {
				player_y -= 4.0;
			}

			if (key[KEY_DOWN] && player_y <= SCREEN_H - PLAYER_SIZE - 4.0) {
				player_y += 4.0;
			}

			if (key[KEY_LEFT] && player_x >= 4.0) {
				player_x -= 4.0;
			}

			if (key[KEY_RIGHT] && player_x <= SCREEN_W - PLAYER_SIZE - 4.0) {
				player_x += 4.0;
			}

			if (key[KEY_SPACE]) {
				for (int i = 0; i < 5; i++) {
					if (!balas[i]->getActive() && !shooted && shootTimer > 35) {
						balas[i]->setActive(true);
						balas[i]->setX(player_x + PLAYER_SIZE);
						balas[i]->setY(player_y + PLAYER_SIZE/2);
						shooted = true;
						shootTimer = 0;
						al_play_sample(hit, 1, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
					}
				}
				shooted = false;
			}

			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = true;
				break;

			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = true;
				break;

			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = true;
				break;

			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = true;
				break;
			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = true;
				break;
			}
		}
		if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = false;
				break;

			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = false;
				break;

			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = false;
				break;

			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = false;
				break;

			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = false;
				break;
			case ALLEGRO_KEY_ESCAPE:
				doexit = true;
				break;
			}
		}

		/*movimiento bala*/
		for (int i = 0; i < 5; i++) {
				balas[i]->BalaUpdate(5, 0);
		}
		/*colisiones jugador-ememigo*/
		for (int i = 0; i < 5; i++){
			if (colision(player_x, player_y, PLAYER_SIZE, enemigos[i]->getX(), enemigos[i]->getY(), ENEMY_SIZE)) {
				int randPosX = rand() % 500 + 750;
				enemigos[i]->setX(randPosX);
				vidas--;
				al_play_sample(hit, 1, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			}
		}
		/*movimiento enemigo*/
		for (int i = 0; i < 5; i++){
			if (enemigos[i]->getX() > player_x) {
				enemigos[i]->setX(enemigos[i]->getX() - 1);
			}
			else enemigos[i]->setX(enemigos[i]->getX() + 1);
			if (enemigos[i]->getY() < player_y) {
				enemigos[i]->setY(enemigos[i]->getY() + 1);
			}
			else enemigos[i]->setY(enemigos[i]->getY() - 1);
		}
		/*Game Over */
		if (vidas <= 0)
			doexit = true;
		/*colisiones bala-enemigo*/
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++){
				if (colision(balas[i]->getX(), balas[i]->getY(), BULLET_SIZE, enemigos[j]->getX(), enemigos[j]->getY(), ENEMY_SIZE)) {
					int randPosX = rand() % 500 + 750;
					enemigos[j]->setX(randPosX);
					balas[i]->setActive(false);
					balas[i]->setX(999);
				}
			}
		}
		/*colision extremos*/
		for (int i = 0; i < 5; i++) {
			if (balas[i]->getX() > SCREEN_W || balas[i]->getX() < 0) {
				balas[i]->setX(999);
				balas[i]->setActive(false);
			}
			else if (balas[i]->getY() > SCREEN_H || balas[i]->getY() < 0) {
				balas[i]->setX(999);
				balas[i]->setActive(false);
			}
		}
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));

			al_draw_bitmap(player, player_x, player_y, 0);

			for (int i = 0; i < 5; i++){
				al_draw_bitmap(enBitmaps[i], enemigos[i]->getX(), enemigos[i]->getY(), 0);
			}
			for (int i = 0; i < 5; i++){
				al_draw_bitmap(bitmaps[i], balas[i]->getX(), balas[i]->getY(), 0);
			}

			al_flip_display();
		}
		shootTimer++;
	}
	al_uninstall_audio();
	al_uninstall_keyboard();
	al_destroy_bitmap(player);
	for (int i = 0; i < 5; i++){
		al_destroy_bitmap(enBitmaps[i]);
	}
	for (int i = 0; i < 5; i++) {
		al_destroy_bitmap(bitmaps[i]);
		delete balas[i];
	}
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}