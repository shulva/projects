#include<string>

class shulva{
    public:
        shulva();
        shulva(int height,int weight,std::string name);
        int get_height() const;
        void set_height(int height);
        int get_weight() const;
        void set_weight(int weight);
    private:
        int height;
        int weight;
        std::string name;
        void set_name(std::string name);
};