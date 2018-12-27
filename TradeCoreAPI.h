
#ifndef TRADERCOREAPI_H_INCLUDED
#define TRADERCOREAPI_H_INCLUDED

#define CANDLESMAX 3000

struct Candle {
  double high;
  double open_price;
  double close_price;
  double low;
};

class TradeCoreAPI {
  public:
    TradeCoreAPI();
    ~TradeCoreAPI();
    void doLoad();
    bool getStatus();
    unsigned int data_size;
    Candle candles[CANDLESMAX];
  private:
    struct MemoryStruct {
      char *memory;
      size_t size;
    };
    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
    void doEraseCandles();
    static const char user_agent[];
    static const char api_host[];
    char *client();
    bool api_status;
    bool res_status;
};

#endif // TRADERCOREAPI_H_INCLUDED
