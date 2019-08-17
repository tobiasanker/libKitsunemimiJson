/**
 *  @file    json_item.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
 */

#include <json_item.h>

#include <data_structure/data_items.h>
#include <json_parsing/json_parser_interface.h>

using Kitsune::Common::DataItem;
using Kitsune::Common::DataArray;
using Kitsune::Common::DataValue;
using Kitsune::Common::DataObject;

namespace Kitsune
{
namespace Json
{

/**
 * @brief JsonItem::JsonItem
 */
JsonItem::JsonItem()
{
    m_content = nullptr;
    m_deletable = true;
}

/**
 * @brief creates a new item based on an already existring item
 *
 * @param otherItem other item, which have to be copied
 */
JsonItem::JsonItem(const JsonItem &otherItem)
{
    m_content = otherItem.m_content->copy();
}

/**
 * @brief creates a new item
 *
 * @param dataItem pointer to an already existring abstract-json
 */
JsonItem::JsonItem(DataItem* dataItem,
                   const bool copy)
{
    if(copy)
    {
        if(dataItem != nullptr) {
            m_content = dataItem->copy();
        }
    }
    else
    {
        m_content = dataItem;
        m_deletable = false;
    }
}

/**
 * @brief creates an object-item
 *
 * @param value map for the new object, which can be empty
 */
JsonItem::JsonItem(std::map<std::string, JsonItem> &value)
{
    DataObject* tempItem = new DataObject();
    m_content = static_cast<DataItem*>(tempItem);

    std::map<std::string, JsonItem>::iterator it;
    for(it = value.begin(); it != value.end(); it++)
    {
        insert(it->first, it->second);
    }
}

/**
 * @brief creates an array-item
 *
 * @param value vector for the new object, which can be empty
 */
JsonItem::JsonItem(std::vector<JsonItem> &value)
{
    DataArray* tempItem = new DataArray();
    m_content = static_cast<DataItem*>(tempItem);

    std::vector<JsonItem>::iterator it;
    for(it = value.begin(); it != value.end(); it++)
    {
        append(*it);
    }
}

/**
 * @brief creates an value-item
 *
 * @param value string of the new item
 */
JsonItem::JsonItem(const std::string &value)
{
    DataValue* tempItem = new DataValue(value);
    m_content = static_cast<DataItem*>(tempItem);
}

/**
 * @brief creates an value-item
 *
 * @param value int-value of the new item
 */
JsonItem::JsonItem(const int value)
{
    DataValue* tempItem = new DataValue(value);
    m_content = static_cast<DataItem*>(tempItem);
}

/**
 * @brief creates an value-item
 *
 * @param value float-value of the new item
 */
JsonItem::JsonItem(const float value)
{
    DataValue* tempItem = new DataValue(value);
    m_content = static_cast<DataItem*>(tempItem);
}

/**
 * destructor
 */
JsonItem::~JsonItem()
{
    clear();
}

/**
 * @brief convert a json-formated string into a json-object-tree
 *
 * @param input json-formated string, which should be parsed
 * @param traceParsing bool-value tracing of parsing-process should be printed
 *
 * @return pair of bool and string
 *         success: first is true, second is empty-string
 *         fail: first is false, second is error-message
 */
std::pair<bool, std::string>
JsonItem::parse(const std::string &input,
                const bool traceParsing)
{
    std::pair<bool, std::string> result;
    JsonParserInterface parser(traceParsing);

    // parse ini-template into a json-tree
    result.first = parser.parse(input);

    // process a failure
    if(result.first == false)
    {
        result.second = parser.getErrorMessage();
        return result;
    }

    clear();

    m_content = parser.getOutput();

    return result;
}

/**
 * @brief replace the content of the item with the content of another item
 *
 * @param other other item, which have to be copied
 *
 * @return pointer to this current item
 */
JsonItem&
JsonItem::operator=(const JsonItem &other)
{
    if(this != &other)
    {
        clear();
        m_content = other.m_content->copy();
    }

    return *this;
}

/**
 * @brief writes a new string into the json-value
 *
 * @param value new string to store
 */
bool
JsonItem::setValue(const std::string &value)
{
    if(m_content == nullptr) {
        m_content = new DataValue();
    }

    if(m_content->getType() == DataItem::VALUE_TYPE)
    {
        m_content->toValue()->setValue(value);
        return true;
    }

    return false;
}

/**
 * @brief writes a new integer into the json-value
 *
 * @param value new number to store
 */
bool
JsonItem::setValue(const int &value)
{
    if(m_content == nullptr) {
        m_content = new DataValue();
    }

    if(m_content->getType() == DataItem::VALUE_TYPE)
    {
        m_content->toValue()->setValue(value);
        return true;
    }

    return false;
}

/**
 * @brief writes a new floaint-point into the json-value
 *
 * @param value new number to store
 */
bool
JsonItem::setValue(const float &value)
{
    if(m_content == nullptr) {
        m_content = new DataValue();
    }

    if(m_content->getType() == DataItem::VALUE_TYPE)
    {
        m_content->toValue()->setValue(value);
        return true;
    }

    return false;
}

/**
 * @brief insert a key-value-pair if the current item is a json-object
 *
 * @param key key of the new pair
 * @param value new json-item-object
 * @param force true to overwrite an existing key (default: false)
 *
 * @return false, if item is not a json-object, else true
 */
bool
JsonItem::insert(const std::string &key,
                 const JsonItem &value,
                 bool force)
{
    if(value.getItemContent() == nullptr
            || key == "")
    {
        return false;
    }

    if(m_content == nullptr) {
        m_content = new DataObject();
    }

    if(m_content->getType() == DataItem::OBJECT_TYPE)
    {
        return m_content->toObject()->insert(key,
                                             value.m_content->copy(),
                                             force);
    }

    return false;
}

/**
 * @brief add a new item, if the current item is a json-array
 *
 * @param value new json-item-object
 *
 * @return false, if item is not a json-array, else true
 */
bool
JsonItem::append(const JsonItem &value)
{
    if(value.getItemContent() == nullptr) {
        return false;
    }

    if(m_content == nullptr) {
        m_content = new DataArray();
    }

    if(m_content->getType() == DataItem::ARRAY_TYPE) {
        return m_content->toArray()->append(value.m_content->copy());
    }

    return false;
}

/**
 * @brief replace an item within a array
 *
 * @param index position in the array
 * @param value new json-item-object
 *
 * @return false, if items not valid of index too high, else true
 */
bool
JsonItem::replaceItem(const uint32_t index,
                      const JsonItem &value)
{
    if(value.getItemContent() == nullptr) {
        return false;
    }

    if(m_content == nullptr) {
        m_content = new DataArray();
    }

    if(m_content->getType() == DataItem::ARRAY_TYPE
            && m_content->toArray()->m_array.size() > index)
    {
        return m_content->toArray()->m_array[index] = value.m_content->copy();
    }

    return false;
}

/**
 * @brief JsonItem::getItemContent
 * @return
 */
DataItem*
JsonItem::getItemContent() const
{
    return m_content;
}

/**
 * @brief get a specific entry of the item
 *
 * @param key key of the requested value
 *
 * @return nullptr if index in key is to high, else object
 */
JsonItem
JsonItem::operator[](const std::string key)
{
    if(m_content == nullptr) {
        return JsonItem();
    }

    return JsonItem(m_content->get(key), true);
}

/**
 * @brief get a specific item of the object
 *
 * @param index index of the item
 *
 * @return nullptr if index is to high, else object
 */
JsonItem
JsonItem::operator[](const uint32_t index)
{
    if(m_content == nullptr) {
        return JsonItem();
    }

    return JsonItem(m_content->get(index), true);
}

/**
 * @brief get a specific entry of the item
 *
 * @param key key of the requested value
 *
 * @return nullptr if index in key is to high, else object
 */
JsonItem
JsonItem::get(const std::string key,
              const bool copy) const
{
    if(m_content == nullptr) {
        return JsonItem();
    }

    return JsonItem(m_content->get(key), copy);
}

/**
 * @brief get a specific item of the object
 *
 * @param index index of the item
 *
 * @return nullptr if index is to high, else object
 */
JsonItem
JsonItem::get(const uint32_t index,
              const bool copy) const
{
    if(m_content == nullptr) {
        return JsonItem();
    }

    return JsonItem(m_content->get(index), copy);
}

/**
 * @brief get string of the item
 *
 * @return string, of the item if int-type, else empty string
 */
std::string
JsonItem::getString() const
{
    if(m_content == nullptr) {
        return "";
    }

    if(m_content->getType() == DataItem::VALUE_TYPE) {
        return m_content->toValue()->toString();
    }

    return "";
}

/**
 * @brief get int-value of the item
 *
 * @return int-value, of the item if int-type, else 0
 */
int
JsonItem::getInt() const
{
    if(m_content == nullptr) {
        return 0;
    }

    if(m_content->toValue()->getValueType() == DataItem::INT_TYPE) {
        return m_content->toValue()->toInt();
    }

    return 0;
}

/**
 * @brief get float-value of the item
 *
 * @return float-value, of the item if float-type, else 0
 */
float
JsonItem::getFloat() const
{
    if(m_content == nullptr) {
        return 0;
    }

    if(m_content->toValue()->getValueType() == DataItem::FLOAT_TYPE) {
        return m_content->toValue()->toFloat();
    }

    return 0;
}

/**
 * @brief getter for the number of elements in the item
 *
 * @return number of elements in the item
 */
uint32_t
JsonItem::getSize() const
{
    if(m_content == nullptr) {
        return 0;
    }

    return m_content->getSize();
}

/**
 * @brief get list of keys if the json-item is an json-object
 *
 * @return string-list with the keys of the map
 */
std::vector<std::string>
JsonItem::getKeys()
{
    if(m_content == nullptr) {
        return std::vector<std::string>();
    }

    if(m_content->getType() == DataItem::OBJECT_TYPE)
    {
        DataObject* obj = static_cast<DataObject*>(m_content);
        return obj->getKeys();
    }

    std::vector<std::string> emptyResult;
    return emptyResult;
}

/**
 * @brief check if a key is in the object-map
 *
 * @param key key-string which should be searched in the map of the object-item
 *
 * @return false if the key doesn't exist or the item is no json-object, else true
 */
bool
JsonItem::contains(const std::string &key)
{
    if(m_content == nullptr) {
        return false;
    }

    if(m_content->getType() == DataItem::OBJECT_TYPE)
    {
        DataObject* obj = static_cast<DataObject*>(m_content);
        return obj->contains(key);
    }

    return false;
}

/**
 * @brief check if the current item is valid
 *
 * @return false, if the item is a null-pointer, else true
 */
bool JsonItem::isValid() const
{
    if(m_content != nullptr) {
        return true;
    }
    return false;
}

/**
 * @brief check if current item is a object
 *
 * @return true if current item is a json-object, else false
 */
bool JsonItem::isObject() const
{
    if(m_content == nullptr) {
        return false;
    }

    if(m_content->getType() == DataItem::OBJECT_TYPE) {
        return true;
    }

    return false;
}

/**
 * @brief check if current item is a array
 *
 * @return true if current item is a json-array, else false
 */
bool JsonItem::isArray() const
{
    if(m_content == nullptr) {
        return false;
    }

    if(m_content->getType() == DataItem::ARRAY_TYPE) {
        return true;
    }

    return false;
}

/**
 * @brief check if current item is a value
 *
 * @return true if current item is a json-value, else false
 */
bool JsonItem::isValue() const
{
    if(m_content == nullptr) {
        return false;
    }

    if(m_content->getType() == DataItem::VALUE_TYPE) {
        return true;
    }

    return false;
}

/**
 * @brief remove an item from the key-value-list
 *
 * @param key key of the pair, which should be deleted
 *
 * @return false if the key doesn't exist, else true
 */
bool
JsonItem::remove(const std::string &key)
{
    if(m_content != nullptr) {
        return m_content->remove(key);
    }

    return false;
}

/**
 * @brief remove an item from the object
 *
 * @param index index of the item
 *
 * @return false if index is to high, else true
 */
bool
JsonItem::remove(const uint32_t index)
{
    if(m_content != nullptr) {
        return m_content->remove(index);
    }
    return false;
}

/**
 * @brief prints the content of the object
 *
 * @param output pointer to the output-string on which the object-content as string will be appended
 */
std::string JsonItem::print(bool indent)
{
    if(m_content != nullptr) {
        return m_content->print(nullptr, indent);
    }
    return "";
}

/**
 * @brief delete the underlaying json-object
 */
void JsonItem::clear()
{
    if(m_content != nullptr
            && m_deletable)
    {
        delete m_content;
        m_content = nullptr;
    }
}

}  // namespace Json
}  // namespace Kitsune