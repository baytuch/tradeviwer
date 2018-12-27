
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl.h>
#include <jansson.h>

#include "TradeCoreAPI.h"


const char TradeCoreAPI::user_agent[] = "TradeCoreClient/1.0";
const char TradeCoreAPI::api_host[] = "http://www.example.com/services/tradecore/api.php";

TradeCoreAPI::TradeCoreAPI(){
  this->api_status = false;
  this->res_status = false;
  this->doEraseCandles();
}

TradeCoreAPI::~TradeCoreAPI(){
}

size_t TradeCoreAPI::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
  size_t realsize = size * nmemb;
  TradeCoreAPI::MemoryStruct * mem = (struct MemoryStruct *) userp;
  mem->memory = (char*) realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL){
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

char *TradeCoreAPI::client(){
  CURL *curl;
  CURLcode res;
  curl_slist *list = NULL;
  TradeCoreAPI::MemoryStruct chunk;
  chunk.memory = (char*) malloc(1);
  chunk.size = 0;
  list = curl_slist_append(list, "Content-Type: application/json");
  this->api_status = false;
  curl = curl_easy_init();
  if(curl){
    curl_easy_setopt(curl, CURLOPT_URL, TradeCoreAPI::api_host);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, TradeCoreAPI::user_agent);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, TradeCoreAPI::WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
    res = curl_easy_perform(curl);
    curl_slist_free_all(list);
    if(res == CURLE_OK){
      if (chunk.size > 0) this->api_status = true;
	  } else {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
    curl_easy_cleanup(curl);
  }
  return chunk.memory;
}

void TradeCoreAPI::doEraseCandles(){
  for (unsigned int i = 0; i < CANDLESMAX; i++){
    this->candles[i].high = 0;
    this->candles[i].open_price = 0;
    this->candles[i].close_price = 0;
    this->candles[i].low = 0;
  }
  this->data_size = 0;
}

void TradeCoreAPI::doLoad(){
  json_t *json;
  json_error_t error;
  unsigned int data_size;
  json_t *tmg_status_obj;
  json_t *data_obj;
  json_t *candle_obj;
  json_t *candle_high;
  json_t *candle_open_price;
  json_t *candle_close_price;
  json_t *candle_low;
  this->res_status = false;
  char *data = this->client();
  json = json_loads(data, 0, &error);
  free(data);
  if (this->api_status){
    if(json){
      if (json_is_object(json)){
        tmg_status_obj = json_object_get(json, "status");
        if (json_is_true(tmg_status_obj)){
          json_object_clear(tmg_status_obj);
          data_obj = json_object_get(json, "data");
          if (json_is_array(data_obj)){
            data_size = 0;
            data_size = json_array_size(data_obj);
            this->doEraseCandles();
            if (data_size > 0 && data_size <= CANDLESMAX){
              for (unsigned int p = 0; p < data_size; p++){
                candle_obj = json_array_get(data_obj, p);
                if (json_is_object(candle_obj)){
                  candle_high = json_object_get(candle_obj, "high");
                  if (json_is_number(candle_high)){
                    this->candles[p].high = (double) json_number_value(candle_high);
                    json_object_clear(candle_high);
                  }
                  candle_open_price = json_object_get(candle_obj, "open_price");
                  if (json_is_number(candle_open_price)){
                    this->candles[p].open_price = (double) json_number_value(candle_open_price);
                    json_object_clear(candle_open_price);
                  }
                  candle_close_price = json_object_get(candle_obj, "close_price");
                  if (json_is_number(candle_close_price)){
                    this->candles[p].close_price = (double) json_number_value(candle_close_price);
                    json_object_clear(candle_close_price);
                  }
                  candle_low = json_object_get(candle_obj, "low");
                  if (json_is_number(candle_low)){
                    this->candles[p].low = (double) json_number_value(candle_low);
                    json_object_clear(candle_low);
                  }
                }
              }
              this->res_status = true;
              this->data_size = data_size;
            }
          }
        }
        json_object_clear(json);
      }
    }
  }
}

bool TradeCoreAPI::getStatus(){
  return this->res_status;
}
