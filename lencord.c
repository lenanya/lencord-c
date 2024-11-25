#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include <nob.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define WIDTH 800
#define HEIGHT 600
#define TITLE "Lencord"
#define FONT_HEIGHT 24
#define BG_COLOR (Color) { 0x20, 0x20, 0x20, 0xff }
#define TEXT_COLOR (Color) { 0x80, 0x80, 0x80, 0xff}
#define RESPONSE_BUFFER_SIZE 128000
#define MESSAGE_BUFFER_SIZE 64000

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} DynamicArray;

size_t ParseMessages(char *Messages, char *ResponseBuffer) {
    char *Temp = malloc(RESPONSE_BUFFER_SIZE);
    Temp = strcpy(Temp, "{\"messages\":");
    Temp = strcat(Temp, ResponseBuffer);
    Temp = strcat(Temp, "}");
    cJSON *ResponseJson = cJSON_Parse(Temp);
    cJSON *MessagesJson = cJSON_GetObjectItem(ResponseJson, "messages");
    char *Printer = cJSON_Print(ResponseJson);
    nob_log(INFO, "%s", Printer);
    return 0;
    const cJSON *ContentJson = NULL;
    const cJSON *MessageJson = NULL;
    cJSON_ArrayForEach(MessageJson, MessagesJson) {
        nob_log(INFO, "reached array loop");
        ContentJson = cJSON_GetObjectItemCaseSensitive(MessageJson, "content");
        if (cJSON_IsString(ContentJson) && (ContentJson->valuestring != NULL)) {
            nob_log(INFO, ContentJson->valuestring);
        } else {
            nob_log(ERROR, "oh man");
        } 
    }
     
    return 0;
}

size_t Callback(char *ReceivedData, size_t Size, size_t nmemb, void *ResponseBufferVoid) {
    size_t RealSize = Size * nmemb;
    char *ResponseBuffer = (char *)ResponseBufferVoid;
    strcat(ResponseBuffer, ReceivedData);
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
    free(Address);
    free(Auth);
    return 0;
}

size_t UpdateMessages(char *ChannelId, char *Token, char *Messages, char *ResponseBuffer) {
    if (GetMessages(ChannelId, Token, ResponseBuffer)) {
        nob_log(ERROR, "GetMessages returned non-zero");
        return 1;
    }
    if (ParseMessages(Messages, ResponseBuffer)) {
        nob_log(ERROR, "ParseMessages returned non-zero");
        return 1;
    }
    Messages = strcpy(Messages, "ur;mom;is;gay;");
    return 0;
}

int main()
{
    char *ResponseBuffer = malloc(RESPONSE_BUFFER_SIZE);
    char *Messages = malloc(MESSAGE_BUFFER_SIZE);

    // Read token file    
    String_Builder StringBuilder = {0};
    const char *TokenFilePath = "token";
    read_entire_file(TokenFilePath, &StringBuilder);
    String_View StringView = sb_to_sv(StringBuilder);
    char *Token = malloc(128);
    sprintf(Token, SV_Fmt, SV_Arg(StringView));

    char *DoccordGeneral = "834799816977416198";

    InitWindow(WIDTH, HEIGHT, TITLE);
    const char *FontFile = "./Iosevka-Regular.ttf";
    Font MainFont = LoadFont(FontFile);
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_HEIGHT);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(TEXT_COLOR));
    GuiSetFont(MainFont);    

    while (!WindowShouldClose())
    {
        
        BeginDrawing();
        ClearBackground(BG_COLOR);
        const Rectangle ListViewRect = {0, 0, WIDTH, (size_t) (HEIGHT * 0.9)};
        if (UpdateMessages(DoccordGeneral, Token, Messages, ResponseBuffer)) {
            nob_log(ERROR, "UpdateMessages returned non-zero");
            free(Messages);
            free(ResponseBuffer);
            free(Token);
            EndDrawing();
            UnloadFont(MainFont);
            CloseWindow();
            return 1;
        };
        int Active = -1;
        GuiListView(ListViewRect, Messages, 0, &Active);        
        ResponseBuffer = calloc(RESPONSE_BUFFER_SIZE, 1);
        Messages = calloc(MESSAGE_BUFFER_SIZE, 1);
        EndDrawing();
    }

    free(Messages);
    free(ResponseBuffer);
    free(Token);
    UnloadFont(MainFont);
    CloseWindow();

    return 0;
}