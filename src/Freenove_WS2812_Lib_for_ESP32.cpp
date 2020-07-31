// 
// 
// 
/**
 * Brief	A library for controlling ws2812 in esp32 platform.
 * Author	SuhaylZhao
 * Company	Freenove
 * Date		2020-07-31
 */


#include "Freenove_WS2812_Lib_for_ESP32.h"


Freenove_ESP32_WS2812::Freenove_ESP32_WS2812(u16 n /*= 8*/, u8 pin_gpio /*= 2*/, u8 chn /*= 0*/, LED_TYPE t /*= TYPE_GRB*/)
{
	ledCounts = n;
	pin = pin_gpio;
	rmt_chn = chn;
	setLedType(t);
}

bool Freenove_ESP32_WS2812::begin()
{
	//config = RMT_DEFAULT_CONFIG_TX(pin, rmt_chn);

	config.rmt_mode = RMT_MODE_TX;
	config.channel = (rmt_channel_t)rmt_chn;
	config.gpio_num = (gpio_num_t)pin;
	config.clk_div = 2;
	config.mem_block_num = 1;

	config.tx_config.carrier_freq_hz = 38000;
	config.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
	config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
	config.tx_config.carrier_duty_percent = 33;
	config.tx_config.carrier_en = false;
	config.tx_config.loop_en = false;
	config.tx_config.idle_output_en = true;

	rmt_config(&config);
	rmt_driver_install(config.channel, 0, 0);

	strip_config = LED_STRIP_DEFAULT_CONFIG(ledCounts, (led_strip_dev_t)config.channel);

	strip = led_strip_new_rmt_ws2812(&strip_config);
	if (!strip) {
		return false;
	}
	return true;
}

void Freenove_ESP32_WS2812::setLedCount(u16 n)
{
	ledCounts = n;
	begin();
}

void Freenove_ESP32_WS2812::setLedType(LED_TYPE t)
{
	rOffset = (t >> 4) & 0x03;
	gOffset = (t >> 2) & 0x03;
	bOffset = t & 0x03;
}

esp_err_t Freenove_ESP32_WS2812::setLedColorData(u8 index, u32 rgb)
{
	return setLedColorData(index, rgb >> 16, rgb >> 8, rgb);
}

esp_err_t Freenove_ESP32_WS2812::setLedColorData(u8 index, u8 r, u8 g, u8 b)
{
	u8 p[3];
	p[rOffset] = r;
	p[gOffset] = g;
	p[bOffset] = b;
	return strip->set_pixel(strip, index, p[0], p[1], p[2]);
}

esp_err_t Freenove_ESP32_WS2812::setLedColor(u8 index, u32 rgb)
{
	return setLedColor(index, rgb >> 16, rgb >> 8, rgb);
}

esp_err_t Freenove_ESP32_WS2812::setLedColor(u8 index, u8 r, u8 g, u8 b)
{
	setLedColorData(index, r, g, b);
	return show();
}

esp_err_t Freenove_ESP32_WS2812::setAllLedsColorData(u32 rgb)
{
	for (int i = 0; i < ledCounts; i++)
	{
		setLedColorData(i, rgb);
	}
	return ESP_OK;
}

esp_err_t Freenove_ESP32_WS2812::setAllLedsColorData(u8 r, u8 g, u8 b)
{
	for (int i = 0; i < ledCounts; i++)
	{
		setLedColorData(i, r, g, b);
	}
	return ESP_OK;
}

esp_err_t Freenove_ESP32_WS2812::setAllLedsColor(u32 rgb)
{
	setAllLedsColorData(rgb);
	show();
}

esp_err_t Freenove_ESP32_WS2812::setAllLedsColor(u8 r, u8 g, u8 b)
{
	setAllLedsColorData(r, g, b);
	show();
}

esp_err_t Freenove_ESP32_WS2812::show()
{
	return strip->refresh(strip, 100);
}

uint32_t Freenove_ESP32_WS2812::Wheel(byte pos)
{
	u32 WheelPos = pos % 0xff;
	if (WheelPos < 85) {
		return ((255 - WheelPos * 3) << 16) | ((WheelPos * 3) << 8);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return (((255 - WheelPos * 3) << 8) | (WheelPos * 3));
	}
	WheelPos -= 170;
	return ((WheelPos * 3) << 16 | (255 - WheelPos * 3));
}
