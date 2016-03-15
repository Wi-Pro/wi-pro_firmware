/*
 * hwm_pub_spi.h
 *
 * Created: 3/14/2016 11:28:18 PM
 *  Author: Brandon Pfeifer 
 */ 


#ifndef HWM_PUB_SPI_H_
#define HWM_PUB_SPI_H_

void HWM_spi_init
	(
	void
	);

int HWM_spi_set_div
	(
	uint8_t division
	);
	
void HWM_spi_write_addr
	(
	uint32_t address
	);
	
void HWM_spi_write_data
	(
	char data
	);

#endif /* HWM_PUB_SPI_H_ */