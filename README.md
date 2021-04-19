# i2c_ld_mp3385agr-driver

Tutorial: i2c Linux driver example for led driver mp3385agr-z

## Device Tree Node

```
&i2c2 {
    clock-frequency = <100000>;
    pinctrl-names = "default";
    pinctrl-0 = <&pinctrl_i2c2>;
    status = "okay";


    led_driver: mp3385agr-z@31 {
        status = "okay";
        compatible = "mp3385agrz";
        reg = <0x31>;
    };
};
```
