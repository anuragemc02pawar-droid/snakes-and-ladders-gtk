#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

int player1 = 1, player2 = 1;
GtkWidget *player1_image, *player2_image, *fixed, *roll_label;


//score file handling

void updateScore(int player) {
    FILE *file;
    int p1, p2;

    // Open the score.txt file for reading
    file = fopen("score.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Read the current scores from the file
    if (fscanf(file, "p1-%d\np2-%d", &p1, &p2) != 2) {
        perror("Error reading scores");
        fclose(file);
        return;
    }
    fclose(file);

    // Update the score based on the input
    if (player == 1) {
        p1 += 1;
    } else if (player == 2) {
        p2 += 1;
    } else {
        printf("Invalid player input. Only 1 or 2 is accepted.\n");
        return;
    }

    // Open the score.txt file for writing
    file = fopen("score.txt", "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    // Write the updated scores back to the file
    fprintf(file, "p1-%d\np2-%d", p1, p2);
    fclose(file);

    printf("Score updated successfully!\n");
}


// Function to roll a six-sided die
int rollDie() {
    return rand() % 6 + 1;
}

// Function to move player images on the board
void movePlayerImage(GtkWidget *player_image, int position) {
    int row = 9-(position - 1) / 10;
    int col = (position - 1) % 10;
    if (row % 2 == 0) { // odd rows go right to left
        col = 9 - col;
    }
    gtk_fixed_move(GTK_FIXED(fixed), player_image, col * 60, row * 60); // Adjust based on image size
}

// Function to update player positions
void updatePlayerPositions() {
    movePlayerImage(player1_image, player1);
    movePlayerImage(player2_image, player2);
}

// Function to move the player
int movePlayer(int currentPlayer, int roll) {
    int newPosition = currentPlayer + roll;
    int snakesAndLadders[101] = {0};
    
    // Snakes
    snakesAndLadders[16] = -13; // Head at 16, tail at 3
    snakesAndLadders[47] = -21; // Head at 47, tail at 26
    snakesAndLadders[49] = -38; // Head at 49, tail at 11
    snakesAndLadders[56] = -3;  // Head at 56, tail at 53
    snakesAndLadders[67] = -11; // Head at 67, tail at 56
    snakesAndLadders[96] = -30; // Head at 96, tail at 66
    snakesAndLadders[90] = -3;  // Head at 90, tail at 87
    snakesAndLadders[83] = -21; // Head at 83, tail at 62

    // Ladders
    snakesAndLadders[5] = 4;    // Bottom at 5, top at 9
    snakesAndLadders[15] = 10;  // Bottom at 15, top at 25
    snakesAndLadders[18] = 62;  // Bottom at 18, top at 80
    snakesAndLadders[44] = 42;  // Bottom at 44, top at 86
    snakesAndLadders[47] = 21;  // Bottom at 47, top at 68
    snakesAndLadders[63] = 15;  // Bottom at 63, top at 78
    snakesAndLadders[71] = 23;  // Bottom at 71, top at 94
    snakesAndLadders[81] = 17;  // Bottom at 81, top at 98

    if (newPosition > 100) {
        return currentPlayer; // Player cannot move beyond square 100
    }

    return newPosition + snakesAndLadders[newPosition];
}

// Function to handle button click for rolling the die
void on_roll_button_clicked(GtkWidget *button, gpointer data) {
    static int currentPlayer = 1;
    static int won = 0;

    if (won) return;

    int roll = rollDie();
    gchar *roll_text = g_strdup_printf("Rolled: %d", roll);
    gtk_label_set_text(GTK_LABEL(roll_label), roll_text);
    g_free(roll_text);

    g_print("Player %d rolled a %d.\n", currentPlayer, roll);

    if (currentPlayer == 1) {
        player1 = movePlayer(player1, roll);
        if (player1 == 100) {
            g_print("Player 1 wins!\n");
            updateScore(1);
            won = 1;
            gtk_label_set_text(GTK_LABEL(roll_label), "Player 1 wins!");
        }
    } else {
        player2 = movePlayer(player2, roll);
        if (player2 == 100) {
            g_print("Player 2 wins!\n");
            updateScore(2);
            won = 1;
            gtk_label_set_text(GTK_LABEL(roll_label), "Player 2 wins!");
        }
    }

    updatePlayerPositions();

    currentPlayer = (currentPlayer == 1) ? 2 : 1;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *board_image, *roll_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Snake and Ladder Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    board_image = gtk_image_new_from_file("C:\\Users\\anura\\OneDrive\\Desktop\\myproject\\build\\board.png");
    if (!board_image) {
        g_print("Failed to load board.png\n");
    }
    gtk_fixed_put(GTK_FIXED(fixed), board_image, 0, 0);

    player1_image = gtk_image_new_from_file("C:\\Users\\anura\\OneDrive\\Desktop\\myproject\\build\\p1.png");
    if (!player1_image) {
        g_print("Failed to load p1.png\n");
    }
    player2_image = gtk_image_new_from_file("C:\\Users\\anura\\OneDrive\\Desktop\\myproject\\build\\p2.png");
    if (!player2_image) {
        g_print("Failed to load p2.png\n");
    }
    gtk_fixed_put(GTK_FIXED(fixed), player1_image, 0, 540); // Start position for Player 1
    gtk_fixed_put(GTK_FIXED(fixed), player2_image, 0, 540); // Start position for Player 2

    roll_button = gtk_button_new_with_label("Roll Die");
    g_signal_connect(roll_button, "clicked", G_CALLBACK(on_roll_button_clicked), NULL);
    gtk_fixed_put(GTK_FIXED(fixed), roll_button, 260, 620);

    roll_label = gtk_label_new("Rolled: ");
    gtk_fixed_put(GTK_FIXED(fixed), roll_label, 320, 620);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    srand(time(0));

    GtkApplication *app;
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return 0;
}
