#include <or1k-support.h>
#include <or1k-sprs.h>
#include <stdint.h>
#include <string.h>

#include "io.h"
#include "gpio.h"
#include "debug.h"
#include "clk.h"
#include "timer.h"
#include "sys.h"
#include "stepgen.h"




int main(void)
{
    enable_caches();
    gpio_init();
    uart0_init();
    clk_set_rate(CLK_CPUS, 300000000);


    // settings for the test ---------------------------------------------------
    #define CH_CNT 8

    struct gpio_t
    {
        int8_t bank;
        int8_t pin;
    };

    const struct gpio_t ch_pin[CH_CNT] =
    {
        /* 3*/{GPIO_BANK_A,12}, /* 5*/{GPIO_BANK_A,11},
        /* 7*/{GPIO_BANK_A, 6}, /* 8*/{GPIO_BANK_A,13},
        /*10*/{GPIO_BANK_A,14}, /*11*/{GPIO_BANK_A, 1},
        /*13*/{GPIO_BANK_A, 0}, /*15*/{GPIO_BANK_A, 3}
    };

    uint32_t ch_freq[CH_CNT] =
    {
        198724,121548,44957,25489,12548,6983,1829,933 // Hz
    };


    timer_start(); // start soft timer

    // set channel pin/task, enable channel
    for( uint8_t c = CH_CNT; c--; )
    {
        ch_set_step_pin(c, ch_pin[c].bank, ch_pin[c].pin);
        ch_set_task(c, ch_freq[c], ch_freq[c]);
        ch_enable(c);
    }


    // main loop ---------------------------------------------------------------
    for ( uint32_t j = 0; ; )
    {
        // if all tasks were done
        if ( !stepgen_loop() )
        {
            printf("job %u done\n", j);
            ++j;

            timer_stop(); // stop soft timer
            delay_us(1000000); // wait a second
            timer_start(); // start soft timer

            // restart all tasks
            for( uint8_t c = CH_CNT; c--; )
            {
                ch_set_task(c, ch_freq[c], ch_freq[c]);
                ch_enable(c);
            }
        }
    }


    return 0;
}

