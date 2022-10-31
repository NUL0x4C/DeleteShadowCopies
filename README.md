### DeleteShadowCopies: Deleting Shadow Copies In Pure C++ 

<br>

### After Looking at some of the leaked ransomware code, i noticed that (at least for the samples i've seen), that the ransomware is using wmic or vssadmin via command line to delete shadow copies, so out of curiosity i had to look for something else, and thus this repo (so im not helping ransomware authers) ...



```
Example:
  - conti: wmic shadowcopy where "ID='{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}'" delete
  - babuk: vssadmin delete shadows /all /quiet

```

## Demo (Creating):
![poc1](https://user-images.githubusercontent.com/111295429/198935990-45b552f9-bce7-44ae-8a91-37f50d81c760.png)

<br>

## Demo (Deleting):
![poc2](https://user-images.githubusercontent.com/111295429/198935994-48041574-4e6b-4a99-b1e0-a6bdfc552a80.png)

<br>

### Based On [vshadow](https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/winbase/vss/vshadow/shadow.cpp)

## Important Note

For educational purposes only. Don't use this code to pursue criminal activity. It should only be be used for ... ransomware simulations, I guess.
