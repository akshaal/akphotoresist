WRITE_CFLAGS$(firmware);

// NOTE: Sometimes it's nice to try to see which one is best to have some not low, must try some combinations
USE_REG$(state);
USE_REG$(akat_timestamp_hour__timestamp, low);
USE_REG$(akat_timestamp_minute__timestamp, low);
USE_REG$(akat_timestamp_second__timestamp);
USE_REG$(akat_timestamp_decisecond__timestamp, low);
USE_REG$(akat_countdown_started__countdown, low);
USE_REG$(buzzer__play_deciseconds, low);
USE_REG$(predef, low);
USE_REG$(button4__delay, low);
USE_REG$(button2__delay, low);
USE_REG$(button1__delay, low);
USE_REG$(tm1637__dirty, low);
USE_REG$(select_first, low);

