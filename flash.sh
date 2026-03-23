# cp ./my_project.uf2 /media/$USER/RPI-RP2/
# sudo picotool reboot -f && sleep 2 && sudo picotool load -f ./my_project.uf2 && sudo picotool reboot
sudo picotool load -f ./my_project.uf2 && sudo picotool reboot

