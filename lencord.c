#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include <nob.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <curl/curl.h>

#define WIDTH 800
#define HEIGHT 600
#define TITLE "Lencord"
#define FONT_HEIGHT 24
#define BG_COLOR (Color) { 0x20, 0x20, 0x20, 0xff }
#define TEXT_COLOR (Color) { 0x80, 0x80, 0x80, 0xff}
#define RESPONSE_BUFFER_SIZE 128000

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} DynamicArray;

void UpdateMessages(char *Messages) {
    const char *New = "Lena: Hi;Urmom: Hi;Goon: Hi";
    const size_t NewSize = strlen(New);
    Messages = realloc(Messages, NewSize);
    strcpy(Messages, New);
}

size_t Callback(char *ReceivedData, size_t Size, size_t nmemb, void *ResponseBufferVoid) {
    size_t RealSize = Size * nmemb;
    char *ResponseBuffer = (char *)ResponseBufferVoid;
    size_t ResponseBufferEnd = strlen(ResponseBuffer);
    size_t ReceivedDataSize = strlen(ReceivedData);
    for (size_t i = 0; i < RealSize; ++i) {
        ResponseBuffer[ResponseBufferEnd + i] = ReceivedData[i];
    }
    return RealSize;
}

size_t GetMessages(char *ChannelId, char *Token, char* ResponseBuffer) {
    CURL *Curl;
    Curl = curl_easy_init();
    if (!Curl) return 1;
    CURLcode ResponseCode;
    struct curl_slist *Headers = NULL;
    char *Address = malloc(sizeof(char) * 0xff);
    char *Auth = malloc(sizeof(char) * 0xff);
    sprintf(Address, "https://discord.com/api/v10/channels/%s/messages", ChannelId);
    sprintf(Auth, "Authorization: %s", Token);
    nob_log(INFO, "%s", Address);
    Headers = curl_slist_append(Headers, Auth);
    curl_easy_setopt(Curl, CURLOPT_URL, Address);
    curl_easy_setopt(Curl, CURLOPT_HTTPHEADER, Headers);
    curl_easy_setopt(Curl, CURLOPT_WRITEFUNCTION, Callback);
    curl_easy_setopt(Curl, CURLOPT_WRITEDATA, (void *)ResponseBuffer);
    ResponseCode = curl_easy_perform(Curl);
    if (ResponseCode != CURLE_OK) {
        nob_log(ERROR, "ResponseCode is non-zero");
        return 1;
    }
    curl_slist_free_all(Headers);
    curl_easy_cleanup(Curl);
    return 0;
}

int main()
{
    char *ResponseBuffer = malloc(RESPONSE_BUFFER_SIZE);

    String_Builder StringBuilder = {0};
    const char *TokenFilePath = "token";
    read_entire_file(TokenFilePath, &StringBuilder);
    String_View StringView = sb_to_sv(StringBuilder);
    char *Token = malloc(128);
    sprintf(Token, SV_Fmt, SV_Arg(StringView));

    char *DoccordGeneral = "834799816977416198";
    if (GetMessages(DoccordGeneral, Token, ResponseBuffer)) {
        nob_log(ERROR, "GetMessages returned non-zero");
        return 1;
    }
    nob_log(INFO, "%s", ResponseBuffer);
    return 0;

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