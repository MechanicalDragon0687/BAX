#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <i2c/i2c.h>

//-----------------------------------------------------------------------------

static const struct { uint8_t bus_id, reg_addr; } dev_data[] = {
    {0, 0x4A}, {0, 0x7A}, {0, 0x78},
    {1, 0x4A}, {1, 0x78}, {1, 0x2C},
    {1, 0x2E}, {1, 0x40}, {1, 0x44},
    {2, 0xD6}, {2, 0xD0}, {2, 0xD2},
    {2, 0xA4}, {2, 0x9A}, {2, 0xA0},
};

inline uint8_t i2cGetDeviceBusId(uint8_t device_id) {
    return dev_data[device_id].bus_id;
}

inline uint8_t i2cGetDeviceRegAddr(uint8_t device_id) {
    return dev_data[device_id].reg_addr;
}

//-----------------------------------------------------------------------------

static volatile uint8_t* reg_data_addrs[] = {
    (volatile uint8_t*)(I2C1_REG_OFF + I2C_REG_DATA),
    (volatile uint8_t*)(I2C2_REG_OFF + I2C_REG_DATA),
    (volatile uint8_t*)(I2C3_REG_OFF + I2C_REG_DATA),
};

inline volatile uint8_t* i2cGetDataReg(uint8_t bus_id) {
    return reg_data_addrs[bus_id];
}

//-----------------------------------------------------------------------------

static volatile uint8_t* reg_cnt_addrs[] = {
    (volatile uint8_t*)(I2C1_REG_OFF + I2C_REG_CNT),
    (volatile uint8_t*)(I2C2_REG_OFF + I2C_REG_CNT),
    (volatile uint8_t*)(I2C3_REG_OFF + I2C_REG_CNT),
};

inline volatile uint8_t* i2cGetCntReg(uint8_t bus_id) {
    return reg_cnt_addrs[bus_id];
}

//-----------------------------------------------------------------------------

inline void i2cWaitBusy(uint8_t bus_id) {
    while (*i2cGetCntReg(bus_id) & 0x80);
}

inline bool i2cGetResult(uint8_t bus_id) {
    i2cWaitBusy(bus_id);
    return (*i2cGetCntReg(bus_id) >> 4) & 1;
}

void i2cStop(uint8_t bus_id, uint8_t arg0) {
    *i2cGetCntReg(bus_id) = (arg0 << 5) | 0xC0;
    i2cWaitBusy(bus_id);
    *i2cGetCntReg(bus_id) = 0xC5;
}

//-----------------------------------------------------------------------------

bool i2cSelectDevice(uint8_t bus_id, uint8_t dev_reg) {
    i2cWaitBusy(bus_id);
    *i2cGetDataReg(bus_id) = dev_reg;
    *i2cGetCntReg(bus_id) = 0xC2;
    return i2cGetResult(bus_id);
}

bool i2cSelectRegister(uint8_t bus_id, uint8_t reg) {
    i2cWaitBusy(bus_id);
    *i2cGetDataReg(bus_id) = reg;
    *i2cGetCntReg(bus_id) = 0xC0;
    return i2cGetResult(bus_id);
}

//-----------------------------------------------------------------------------

uint8_t i2cReadRegister(uint8_t dev_id, uint8_t reg) {
    uint8_t bus_id = i2cGetDeviceBusId(dev_id);
    uint8_t dev_addr = i2cGetDeviceRegAddr(dev_id);

    for (size_t i = 0; i < 8; i++) {
        if (i2cSelectDevice(bus_id, dev_addr) && i2cSelectRegister(bus_id, reg)) {
            if (i2cSelectDevice(bus_id, dev_addr | 1)) {
                i2cWaitBusy(bus_id);
                i2cStop(bus_id, 1);
                i2cWaitBusy(bus_id);
                return *i2cGetDataReg(bus_id);
            }
        }
        *i2cGetCntReg(bus_id) = 0xC5;
        i2cWaitBusy(bus_id);
    }
    return 0xff;
}

bool i2cReadRegisterBuffer(unsigned int dev_id, int reg, uint8_t* buffer, size_t buf_size) {
    uint8_t bus_id = i2cGetDeviceBusId(dev_id);
    uint8_t dev_addr = i2cGetDeviceRegAddr(dev_id);

    size_t j = 0;
    while (!i2cSelectDevice(bus_id, dev_addr)
        || !i2cSelectRegister(bus_id, reg)
        || !i2cSelectDevice(bus_id, dev_addr | 1))
    {
        i2cWaitBusy(bus_id);
        *i2cGetCntReg(bus_id) = 0xC5;
        i2cWaitBusy(bus_id);
        if (++j >= 8)
            return false;
    }

    if (buf_size != 1) {
        for (size_t i = 0; i < buf_size - 1; i++) {
            i2cWaitBusy(bus_id);
            *i2cGetCntReg(bus_id) = 0xF0;
            i2cWaitBusy(bus_id);
            buffer[i] = *i2cGetDataReg(bus_id);
        }
    }

    i2cWaitBusy(bus_id);
    *i2cGetCntReg(bus_id) = 0xE1;
    i2cWaitBusy(bus_id);
    buffer[buf_size - 1] = *i2cGetDataReg(bus_id);
    return true;
}

bool i2cWriteRegister(uint8_t dev_id, uint8_t reg, uint8_t data) {
    uint8_t bus_id = i2cGetDeviceBusId(dev_id);
    uint8_t dev_addr = i2cGetDeviceRegAddr(dev_id);

    for (int i = 0; i < 8; i++) {
        if (i2cSelectDevice(bus_id, dev_addr) && i2cSelectRegister(bus_id, reg)) {
            i2cWaitBusy(bus_id);
            *i2cGetDataReg(bus_id) = data;
            *i2cGetCntReg(bus_id) = 0xC1;
            i2cStop(bus_id, 0);
            if (i2cGetResult(bus_id))
                return true;
        }
        *i2cGetCntReg(bus_id) = 0xC5;
        i2cWaitBusy(bus_id);
    }

    return false;
}
