/********************|
|***** PONG v1.3 ****|
|**** WRITTEN FOR ***|
|***** SWINGAME *****|
|* JOEL ROZEN-OXLEY *|
|****** 7536372 *****|
|****** 20110913 ****|
|********************/

// Includes
#include <stdio.h>
#include <stdbool.h>
#include "SwinGame.h"
#include <string.h>

// Create structures for the paddle, ball, scores and overall game
typedef struct {
	int length;
	int width;
	int x_position;
	float old_y_position;
	float new_y_position;
	float max_speed;
	float delta_y;
} paddle;

typedef struct {
	int radius;
	int delta_x;
	int delta_y;
	int max_speed;
	int old_x_centre;
	int new_x_centre;
	int old_y_centre;
	int new_y_centre;
} ball;

typedef struct {
	int score;
} score;

typedef struct {
	paddle paddles[2];
	ball game_ball;
	score scores[2];
	bool victory;
	int difficulty;
} game;

void load_resources() {
	load_default_colors();
	load_sound_effect_named("hit", "dit.wav");
	load_sound_effect_named("score", "dah.wav");
}

void display_difficulty_options() {
	char tempstring[22];
	sprintf(tempstring, "Please Press a Number");
	draw_text_on_screen(tempstring, ColorWhite, (screen_width()/2) - (strlen(tempstring)*8)/2, 50);
	sprintf(tempstring, "1 - Easy");
	draw_text_on_screen(tempstring, ColorWhite, (screen_width()/2) - (strlen(tempstring)*8)/2, 75);
	sprintf(tempstring, "2 - Medium");
	draw_text_on_screen(tempstring, ColorWhite, (screen_width()/2) - (strlen(tempstring)*8)/2, 100);
	sprintf(tempstring, "3 - Hard");
	draw_text_on_screen(tempstring, ColorWhite, (screen_width()/2) - (strlen(tempstring)*8)/2, 125);
	sprintf(tempstring, "4 - Two Player");
	draw_text_on_screen(tempstring, ColorWhite, (screen_width()/2) - (strlen(tempstring)*8)/2, 150);
	refresh_screen();
}

void handle_selection(game &game) {

	do {
		process_events();
		if (key_typed(VK_1)) {
			game.difficulty = 1;
			break;
		}
		if (key_typed(VK_2)) {
			game.difficulty = 2;
			break;
		}
		if (key_typed(VK_3)) {
			game.difficulty = 3;
			break;
		}
		if (key_typed(VK_4)) {
			game.difficulty = 4;
			break;
		}
		
	} while (!key_typed(VK_1) && !key_typed(VK_2) && !key_typed(VK_3) && !key_typed(VK_4));
	
	
	switch (game.difficulty) {
		case 1:
			game.paddles[0].max_speed = 2;
			break;
		case 2:
			game.paddles[0].max_speed = 5;
			break;
		case 3:
			game.paddles[0].max_speed = 8;
			break;
		case 4:
			game.paddles[0].max_speed = 5;
			break;
		default:
			game.paddles[0].max_speed = 5;
			break;
	}	
}

// procedure to generate a random direction and speed
// that the ball will begin with
void generate_random_direction(game &game) {
	float outcome;
	
	switch (game.difficulty) {
		case 1:
			outcome = rnd() * 16 - 8;
			game.game_ball.delta_y = outcome;
			
			outcome = rnd() * 16 - 8;
			if (outcome > -1 && outcome < 1) {
				if (outcome <= 0) outcome += -2;
				if (outcome > 0) outcome += 2;
			}
			game.game_ball.delta_x = outcome;
			
			game.game_ball.max_speed = 8;
			break;
			
		case 2:
			outcome = rnd() * 20 - 10;
			game.game_ball.delta_y = outcome;
			
			outcome = rnd() * 20 - 10;
			if (outcome > -1 && outcome < 1) {
				if (outcome <= 0) outcome += -2;
				if (outcome > 0) outcome += 2;
			}
			game.game_ball.delta_x = outcome;
			
			game.game_ball.max_speed = 10;
			break;

		case 3:
			outcome = rnd() * 24 - 12;
			game.game_ball.delta_y = outcome;
			
			outcome = rnd() * 24 - 12;
			if (outcome > -1 && outcome < 1) {
				if (outcome <= 0) outcome += -2;
				if (outcome > 0) outcome += 2;
			}
			game.game_ball.delta_x = outcome;
			
			game.game_ball.max_speed = 12;
			break;

		case 4:
			outcome = rnd() * 20 - 10;
			game.game_ball.delta_y = outcome;
			
			outcome = rnd() * 20 - 10;
			if (outcome > -1 && outcome < 1) {
				if (outcome <= 0) outcome += -2;
				if (outcome > 0) outcome += 2;
			}
			game.game_ball.delta_x = outcome;
			
			game.game_ball.max_speed = 10;
			break;
		
		default:
			outcome = rnd() * 20 - 10;
			game.game_ball.delta_y = outcome;
			
			outcome = rnd() * 20 - 10;
			if (outcome > -1 && outcome < 1) {
				if (outcome <= 0) outcome += -2;
				if (outcome > 0) outcome += 2;
			}
			game.game_ball.delta_x = outcome;
			
			game.game_ball.max_speed = 10;
			break;
	}
}

// procedure to move the game_ball.
void move_ball(ball &game_ball) {
	game_ball.old_x_centre = game_ball.new_x_centre;
	game_ball.old_y_centre = game_ball.new_y_centre;
	game_ball.new_x_centre += game_ball.delta_x;
	game_ball.new_y_centre += game_ball.delta_y;

	// bounce ball off the top of the windows
	if ( (game_ball.new_y_centre - game_ball.radius) < 25) {
		game_ball.delta_y = -game_ball.delta_y;
	}
	// bounce ball off the bottom of the window
	if ( (game_ball.new_y_centre + game_ball.radius) > (screen_height() - 25) ) {
		game_ball.delta_y = -game_ball.delta_y;
	}
}

// procedure to draw the ball, given a position.
void draw_ball(ball &game_ball) {
	fill_circle(ColorWhite, game_ball.new_x_centre, game_ball.new_y_centre, game_ball.radius);
}

// responds to user input and adds/substracts from each paddles' delta_y, given a specific command
void handle_input(paddle paddles[]) {

	if ( key_down(VK_UP) && paddles[1].delta_y >= -paddles[1].max_speed ) {
		paddles[1].delta_y -= 0.5;
	}  else if ( key_down(VK_DOWN) && paddles[1].delta_y <= paddles[1].max_speed ) {
		paddles[1].delta_y += 0.5;
	}
	if ( ( !key_down(VK_UP) && !key_down(VK_DOWN) ) && paddles[1].delta_y != 0 ) {
		if ( paddles[1].delta_y > 0 ) paddles[1].delta_y -= 0.5;
		if ( paddles[1].delta_y < 0 ) paddles[1].delta_y += 0.5;
	}
}

void update_ai(paddle &ai, ball &b) {
	if( (b.new_y_centre + b.radius) < (ai.new_y_position + (ai.length/2) - (ai.length/3)) && ai.delta_y >= -ai.max_speed ) {
		ai.delta_y -= 0.5;
	} else if ( (b.new_y_centre - b.radius) > (ai.new_y_position + (ai.length/2) + (ai.length/3)) && ai.delta_y <= ai.max_speed ) {
		ai.delta_y += 0.5;
	}
	if( !((b.new_y_centre + b.radius) < (ai.new_y_position + (ai.length/2)-(ai.length/3))) && !((b.new_y_centre - b.radius) > (ai.new_y_position + (ai.length/2) + (ai.length/3))) && ai.delta_y != 0 ) {
		if (ai.delta_y > 0 ) ai.delta_y -= 0.5;
		if (ai.delta_y < 0 ) ai.delta_y += 0.5;
	}
}

// handles individual paddle movement
void move_paddle(paddle &p) {
	if (p.delta_y == 0) return;

	p.new_y_position += p.delta_y;

	if ( p.new_y_position < 25 ) p.new_y_position = 25;
	else if ( p.new_y_position > screen_height() - 25 - p.length ) p.new_y_position = screen_height() - 25 - p.length;
}

void move_paddles(paddle p[]) {
	move_paddle(p[0]);
	move_paddle(p[1]);
}

// procedure to draw paddles, given the variables in the paddle structure
void draw_paddles(paddle paddles[], int count) {
	int i = 0;
	while (i < count) {
		fill_rectangle(ColorWhite, paddles[i].x_position, paddles[i].new_y_position, paddles[i].width, paddles[i].length);
		i++;
	}
}

// procedure to draw the court borders and centre line.
void draw_court() {
	draw_rectangle(ColorWhite, 5, 25, screen_width() - 10, screen_height() - 50);
	draw_line(ColorWhite, (screen_width()/2), 25, (screen_width()/2), screen_height() - 25);
}

// displays the current score above the respective players paddles.
void display_scores(score scores[]) {
	char tempstring[15];
	sprintf(tempstring, "Player A - %i", scores[0].score);
	draw_text_on_screen(tempstring, ColorWhite, 5, 2);
	sprintf(tempstring, "Player B - %i", scores[1].score);
	draw_text_on_screen(tempstring, ColorWhite, screen_width() - 102, 2);
}

// determine whether or not a score of atleast 7 has been achieved, and whether or not they have
// won by 2 or more points.
void detect_victory(game &game) {

	char tempstring[13];

	if (game.scores[0].score >= 7 && (game.scores[0].score - game.scores[1].score) >= 2) {
		clear_screen();
		sprintf(tempstring, "Player A Won!");
		draw_text_on_screen(tempstring, ColorWhite, screen_width()/2 - 35, screen_height()/2 - 10);
		refresh_screen();
		game.victory = true;
		delay(2000);
	}
	else if (game.scores[1].score >= 7 && (game.scores[1].score - game.scores[0].score) >= 2) {
		clear_screen();
		sprintf(tempstring, "Player B Won!");
		draw_text_on_screen(tempstring, ColorWhite, screen_width()/2 - 35, screen_height()/2 - 10);
		refresh_screen();
		game.victory = true;
		delay(2000);
	}
}

// initialise all variables
void init_game(game &game) {

	game.paddles[0].length = 100;
	game.paddles[0].width = 4;
	game.paddles[0].x_position = 10;
	game.paddles[0].old_y_position = (screen_height()/2)-(game.paddles[0].length/2);
	game.paddles[0].new_y_position = (screen_height()/2)-(game.paddles[0].length/2);	

	game.paddles[1].length = 100;
	game.paddles[1].width = 4;
	game.paddles[1].x_position = screen_width() - 10 - game.paddles[1].width;
	game.paddles[1].old_y_position = (screen_height()/2)-(game.paddles[1].length/2);
	game.paddles[1].new_y_position = (screen_height()/2)-(game.paddles[1].length/2);
	game.paddles[1].max_speed = 5;

	game.game_ball.radius = 5;
	generate_random_direction(game);
	game.game_ball.old_x_centre = screen_width()/2;
	game.game_ball.new_x_centre = screen_width()/2;
	game.game_ball.old_y_centre = screen_height()/2;
	game.game_ball.new_y_centre = screen_height()/2;

	game.scores[0].score = 0;
	game.scores[1].score = 0;

	game.victory = false;
}

// resets the ball variables when a point is scored
void update_game(game &game) {

	generate_random_direction(game);
	game.game_ball.new_x_centre = screen_width()/2;
	game.game_ball.new_y_centre = screen_width()/2;

}

// works out whether the ball has hit a paddle, or gone past it.
void collision_detect(game &game) {
	float outcome;
	if ( (game.game_ball.new_x_centre + game.game_ball.radius) > game.paddles[1].x_position) { // if edge of ball gets to the same x position as edge of RHS paddle
		if (game.game_ball.new_y_centre >= game.paddles[1].new_y_position && game.game_ball.new_y_centre < game.paddles[1].new_y_position + game.paddles[1].length) {
			game.game_ball.delta_x = -game.game_ball.delta_x;
			
			switch (game.difficulty) {
				case 1:
					outcome = rnd() * 8;
					game.game_ball.delta_y = outcome;
					break;
					
				case 2:
					outcome = rnd() * 10;
					game.game_ball.delta_y = outcome;
					break;
					
				case 3:
					outcome = rnd() * 12;
					game.game_ball.delta_y = outcome;
					break;
					
				case 4:
					outcome = rnd() * 10;
					game.game_ball.delta_y = outcome;
					break;
					
				default:
					outcome = rnd() * 20 - 10;
					game.game_ball.delta_y = outcome;
					
					outcome = rnd() * 20 - 10;
					if (outcome > -1 && outcome < 1) {
						if (outcome <= 0) outcome += -2;
						if (outcome > 0) outcome += 2;
					}
					game.game_ball.delta_x = outcome;
					
					game.game_ball.max_speed = 10;
					break;
			}
			
			
			play_sound_effect("hit");
			if (game.game_ball.delta_y * 10 % 2 == 0) {
				game.game_ball.delta_y = - game.game_ball.delta_y; // randomnly reverse the y direction of deflection.
			}
		} else {
			game.scores[0].score ++; // if it missed the paddle, give 1 point to other side
			play_sound_effect("score");
			update_game(game); // reset the ball and paddle positions
		}
		return;
	}

	// same thing but for LHS paddle.
	else if ( (game.game_ball.new_x_centre - game.game_ball.radius) <= game.paddles[0].x_position + game.paddles[0].width) {
		if (game.game_ball.new_y_centre > game.paddles[0].new_y_position && game.game_ball.new_y_centre < game.paddles[0].new_y_position + game.paddles[0].length) {
			game.game_ball.delta_x = - game.game_ball.delta_x;
			game.game_ball.delta_y = rnd () * 7;
			play_sound_effect("hit");
			if (game.game_ball.delta_y * 10 % 2 == 0) {
				game.game_ball.delta_y = - game.game_ball.delta_y;
			}
		} else {
			game.scores[1].score ++;
			play_sound_effect("score");
			update_game(game);
		}
		return;
	}
}

void play_game(game &game) {

	hide_mouse();

	draw_court();

	move_ball(game.game_ball); // initialises ball movement
	draw_ball(game.game_ball); // draws the ball to the screen

	handle_input(game.paddles); // updates delta value based on input
	update_ai(game.paddles[0], game.game_ball);
	move_paddles(game.paddles);// updates paddle position based on delta value
	draw_paddles(game.paddles, 2); // draws the paddles to the screen
	collision_detect(game); // initialises scoring and reset system
	display_scores(game.scores); // draws the scores to the screen
	detect_victory(game); // checks to see if a player has won
	
}

int main() {
	game game;
	open_audio();
	
	open_graphics_window("SwinGame - Pong", 1440, 800);
	load_resources();
	
	init_game(game); // load starting variables (needs to be outside the loop)

	display_difficulty_options();
	handle_selection(game);


	do {
	process_events();
	clear_screen(); // clears the screen

	play_game(game);

	refresh_screen(); // refresh and process the events.
	
	} while ( ! window_close_requested() && ! key_typed(VK_ESCAPE) && game.victory != true );

	close_audio();
	release_all_resources();

	return 0;
}