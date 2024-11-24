#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include <nob.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#define WIDTH 800
#define HEIGHT 600
#define TITLE "Lencord"
#define FONT_HEIGHT 24
#define BG_COLOR (Color) { 0x20, 0x20, 0x20, 0xff }
#define TEXT_COLOR (Color) { 0x80, 0x80, 0x80, 0xff}

void UpdateMessages(char *Messages) {
    const char *New = "Lena: Hi;Urmom: Hi;Goon: Hi";
    const size_t NewSize = strlen(New);
    Messages = realloc(Messages, NewSize);
    strcpy(Messages, New);
}

int main()
{
    InitWindow(WIDTH, HEIGHT, TITLE);
    const char *FontFile = "./Iosevka-Regular.ttf";
    Font MainFont = LoadFont(FontFile);
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_HEIGHT);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(TEXT_COLOR));
    GuiSetFont(MainFont);

    char *Messages = malloc(sizeof(char) * 0xff);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BG_COLOR);
        const Rectangle ListViewRect = {0, 0, WIDTH, (size_t) (HEIGHT * 0.9)};
        UpdateMessages(Messages);
        int Active = -1;
        GuiListView(ListViewRect, Messages, 0, &Active);        
        
        EndDrawing();
    }

    UnloadFont(MainFont);
    CloseWindow();

    return 0;
}