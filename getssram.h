#include <EEPROM.h>
int startAddres = 0, addres = startAddres;
int putMemo(int memory, float value_name)
{
    EEPROM.put(memory, value_name);
    if (addres += sizeof(float) > EEPROM.length())
    {
        addres = startAddres;
    }
}
int getMemo(int addres, float value_name)
{
    EEPROM.get(addres, value_name);
}
