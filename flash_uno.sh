sudo dfu-programmer atmega16u2 erase
echo "erased"
sudo dfu-programmer atmega16u2 flash Joystick.hex
echo "Flashed Joystick.hex"
sudo dfu-programmer atmega16u2 reset
echo "Reset Uno. Done!"
