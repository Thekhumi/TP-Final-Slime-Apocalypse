#ifndef _DEBUG
#define ALLEGRO_STATICLINK
#endif // !_DEBUG
#define PLAYER_FILE "player.png"
#define ENEMY_FILE "enemy.png"
#define BULLET_FILE "bullet.png"
#define MENU_FILE "menu.png"
#define HIT_FILE "Hit.ogg"
#define BACKGROUND_FILE "Background.png"
#define SNIPER_FILE "Sniper_Rifle.ogg"
#define ASSAULT_FILE "Assault_Riffle.ogg"
#define MUSIC_FILE "music.ogg"
#define TOPE 7
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

struct armas { 
int reloadspeed; 
int bulletSpeed; 
int damage; 
ALLEGRO_SAMPLE *sound;
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
	int lanes[3] = { SCREEN_H - ENEMY_SIZE - 100,SCREEN_H/2 - ENEMY_SIZE/2, 0 + 100 };		// Lineas en Y
	struct armas pistol;
	pistol.bulletSpeed = 12;
	pistol.damage = 1;
	pistol.reloadspeed = 35;
	struct armas sniper;
	sniper.bulletSpeed = 20;
	sniper.damage = 3;
	sniper.reloadspeed = 55;
	struct armas rifle;
	rifle.bulletSpeed = 15;
	rifle.damage = 1;
	rifle.reloadspeed = 15;
	struct armas arrArmas[3] = { pistol,sniper,rifle };
	int currentGun = 0;
	srand(time(0));
	ALLEGRO_DISPLAY *display = NULL;					// pantalla
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;			//cola de eventos
	ALLEGRO_TIMER *timer = NULL;						//timer de juego
	ALLEGRO_BITMAP *player = NULL;						//bitmap de player
	ALLEGRO_BITMAP *menu = NULL;
	ALLEGRO_BITMAP *background = NULL;
	float player_x = 0;									//posicion en x de player
	float player_y = lanes[1];							//posicion en y de player
	Enemigo* enemigos[TOPE];
	ALLEGRO_BITMAP * enBitmaps[TOPE];						// bitmap de enemigo
	for (int i = 0; i < TOPE; i++)
	{
		int randPosX = rand() % 1000 + 700;
		int randPosY = rand() % 3 + 0;
		int randSpeed = rand() % 2 + 2;
		enemigos[i] = new Enemigo(randPosX,lanes[randPosY], randSpeed);
		enBitmaps[i] = NULL;
	}
	int vidas = 3;
	int shootTimer = 0;
	bool shooted = false;
	int switchTimer = 0;
	bool redraw = true;
	bool doexit = false;
	bool key[5] = { false, false, false, false, false };		//arreglo de teclas
	Bala* balas[10];
	ALLEGRO_BITMAP * bitmaps[10];
	/* Inicializar Balas */
	for (int i = 0; i < 10; i++)
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
	for (int i = 0; i < TOPE; i++){
		enBitmaps[i] = al_load_bitmap(ENEMY_FILE);
	}
	menu = al_load_bitmap(MENU_FILE);
	al_reserve_samples(10);
	ALLEGRO_SAMPLE *hit = al_load_sample(HIT_FILE);
	ALLEGRO_SAMPLE *sniperShoot = al_load_sample(SNIPER_FILE);
	ALLEGRO_SAMPLE *assaultShoot = al_load_sample(ASSAULT_FILE);
	ALLEGRO_SAMPLE *music = al_load_sample(MUSIC_FILE);
	arrArmas[0].sound = hit;
	arrArmas[1].sound = sniperShoot;
	arrArmas[2].sound = assaultShoot;
	for (int i = 0; i < 10; i++) {
		bitmaps[i] = al_load_bitmap(BULLET_FILE);
	}
	background = al_load_bitmap(BACKGROUND_FILE);
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
	al_play_sample(music,1, 0, 1.0, ALLEGRO_PLAYMODE_LOOP, 0);
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


			if (key[KEY_LEFT] && switchTimer > 35) {
				if (currentGun == 0) {
					currentGun = 2;
				}
				else {
					currentGun--;
				}
				switchTimer = 0;
			}

			if (key[KEY_RIGHT] && switchTimer > 35) {
				if (currentGun == 2) {
					currentGun = 0;
				}
				else {
					currentGun++;
				}
				switchTimer = 0;
			}

			if (key[KEY_SPACE]) {
				for (int i = 0; i < 10; i++) {
					if (!balas[i]->getActive() && !shooted && shootTimer > arrArmas[currentGun].reloadspeed) {
						balas[i]->setActive(true);
						balas[i]->setX(player_x + PLAYER_SIZE);
						balas[i]->setY(player_y + PLAYER_SIZE/2);
						balas[i]->setDamage(arrArmas[currentGun].damage);
						shooted = true;
						shootTimer = 0;
						al_play_sample(arrArmas[currentGun].sound, 0.5, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
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
				balas[i]->BalaUpdate(arrArmas[currentGun].bulletSpeed + 5, 0);
		}
		/*colisiones jugador-ememigo*/
		for (int i = 0; i < TOPE; i++){
			if (enemigos[i]->getX() < 35) {
				int randPosX = rand() % 1000 +	750;
				int randPosY = rand() % 3 + 0;
				enemigos[i]->setX(randPosX);
				enemigos[i]->setY(lanes[randPosY]);
				vidas--;
				al_play_sample(hit, 1, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			}
		}
		/*movimiento enemigo*/
		for (int i = 0; i < TOPE; i++){
			if (enemigos[i]->getX() > player_x) {
				enemigos[i]->setX(enemigos[i]->getX() - enemigos[i]->getSpeed());
			}
			
		}
		/*Game Over */
		if (vidas <= 0)
			doexit = true;
		/*colisiones bala-enemigo*/
		for (int i = 0; i < 10; i++) {
			if(balas[i]->getActive())
			for (int j = 0; j < TOPE; j++){
				if (colision(balas[i]->getX(), balas[i]->getY(), BULLET_SIZE, enemigos[j]->getX(), enemigos[j]->getY(), ENEMY_SIZE)) {
					int randPosX = rand() % 500 + 750;
					int randPosY = rand() % 3 + 0;
					enemigos[j]->setX(randPosX);
					enemigos[j]->setY(lanes[randPosY]);
					balas[i]->setDamage(balas[i]->getDamage() - 1);
					if (balas[i]->getDamage() < 1) {
						balas[i]->setActive(false);
						balas[i]->setX(999);
					}
				}
			}
		}
		/*colision extremos*/
		for (int i = 0; i < 10; i++) {
			if (balas[i]->getX() > SCREEN_W || balas[i]->getX() < 0) {
				balas[i]->setX(999);
				balas[i]->setActive(false);
			}
			else if (balas[i]->getY() > SCREEN_H || balas[i]->getY() < 0) {
				balas[i]->setX(999);
				balas[i]->setActive(false);
			}
		}
		/* Dibujar */
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			al_draw_bitmap(background,0,0,0);

			al_draw_bitmap_region(player, 1, 32 * currentGun, 32, 32, player_x, player_y, 0);

			for (int i = 0; i < TOPE; i++){
				al_draw_bitmap(enBitmaps[i], enemigos[i]->getX(), enemigos[i]->getY(), 0);
			}
			for (int i = 0; i < 10; i++){
				al_draw_bitmap(bitmaps[i], balas[i]->getX(), balas[i]->getY(), 0);
			}
			al_flip_display();
		}
		shootTimer++;
		switchTimer++;
	}
	al_uninstall_audio();
	al_uninstall_keyboard();
	al_destroy_bitmap(player);
	for (int i = 0; i < TOPE; i++){
		al_destroy_bitmap(enBitmaps[i]);
	}
	for (int i = 0; i < 10; i++) {
		al_destroy_bitmap(bitmaps[i]);
		delete balas[i];
	}
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}