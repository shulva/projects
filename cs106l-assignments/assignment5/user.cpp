#include "user.h"

/**
 * Creates a new User with the given name and no friends.
 */
User::User(const std::string& name)
  : _name(name)
  , _friends(nullptr)
  , _size(0)
  , _capacity(0)
{
}

/**
 * Adds a friend to this User's list of friends.
 * @param name The name of the friend to add.
 */
void
User::add_friend(const std::string& name)
{
  if (_size == _capacity) {
    _capacity = 2 * _capacity + 1;
    std::string* newFriends = new std::string[_capacity];
    for (size_t i = 0; i < _size; ++i) {
      newFriends[i] = _friends[i];
    }
    delete[] _friends;
    _friends = newFriends;
  }

  _friends[_size++] = name;
}

/**
 * Returns the name of this User.
 */
std::string
User::get_name() const
{
  return _name;
}

/**
 * Returns the number of friends this User has.
 */
size_t
User::size() const
{
  return _size;
}

/**
 * Sets the friend at the given index to the given name.
 * @param index The index of the friend to set.
 * @param name The name to set the friend to.
 */
void User::set_friend(size_t index, const std::string& name)
{
  _friends[index] = name;
}

//------------------------------------------------------------------------

std::ostream& operator<<  (std::ostream& os,const User& user) {
    os<<"User(name="<<user._name<<", friends=[";

    for(size_t i=0;i<user.size();i++){
        os<<user._friends[i];

        if(i<user.size()-1)
            os<<", ";
    }

    os<<"])";
    return os;
}

User::~User() {
    delete[] _friends;
}

User::User(const User &other_user):_size(other_user._size),_capacity(other_user._capacity),_name(other_user._name),_friends(new std::string[other_user._capacity]) {
    for (size_t i = 0; i <_size ; ++i) {
        _friends[i]=other_user._friends[i];
    }
}

User& User::operator=(const User &other_user) {

    delete[] _friends;

    _capacity = other_user._capacity;
    _size = other_user._size;
    _name = other_user._name;

    if(other_user._friends!= nullptr){
        _friends = new std::string [_size];

        for (size_t i = 0; i <_size ; ++i) {
            _friends[i]=other_user._friends[i];
        }
    }
    else{
        _friends= nullptr;
    }

    return *this;
}

User& User::operator+=(User& rhs){
    rhs.add_friend(_name);
    this->add_friend(rhs.get_name());
    return *this;
}

bool User::operator<(const User& rhs) const{
    return this->_name<rhs._name;
}

