for (int j = 0, k = 0; filename_rough[j] != '\0'; j++, k++){
        if (filename_rough[j] == '%' && j <= filename_size - 3){ //char is '%' and enough room to replace "%20" with ' '
            filename[k] = ' ';
            j += 2; //iterate past "%2"
        } else {
            filename[k] = filename_rough[j];
        }
    }