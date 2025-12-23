#include "class.h"

shulva::shulva() {
    this->height = 170;
    this->weight = 60;
    this->name = "shulva";
}

shulva::shulva(int height, int weight, std::string name) {
    this->height = height;
    this->weight = weight;
    this->name = name;
}

int shulva::get_height() const {
    return this->height;
}

int shulva::get_weight() const{
    return weight;
}

void shulva::set_height(int height) {
    this->height = height;
}

void shulva::set_weight(int weight) {
    this->weight = weight;
}

void shulva::set_name(std::string name) {
    this->name = name;
}
