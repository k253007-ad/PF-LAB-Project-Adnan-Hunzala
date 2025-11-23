#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_WIDTH 200
#define MAP_HEIGHT 100

void LoadCSVToArray(const char* filename, int array[MAP_HEIGHT][MAP_WIDTH], int* rows, int* cols) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return;
    }
    
    char line[1024];
    int row = 0;
    
    while (fgets(line, sizeof(line), file) && row < MAP_HEIGHT) {
        int col = 0;
        char* token = strtok(line, ",");
        
        while (token != NULL && col < MAP_WIDTH) {
            array[row][col] = atoi(token);
            col++;
            token = strtok(NULL, ",");
        }
        
        if (row == 0) *cols = col;
        row++;
    }
    
    *rows = row;
    fclose(file);
}

int levelMap[MAP_HEIGHT][MAP_WIDTH] = {0};
int rows, cols;

typedef struct{ 
    int tileX;
    int tileY;
    bool solid;
}TileType;

typedef struct {
    int x, y;
    int type;
} Tile;

TileType tileTypes[] = {
    {0,  0, true},   // Type 0
    {1,  0, true},   // Type 1
    {2,  0, true},   // Type 2
    {3,  0, true},   // Type 3
    {4,  0, true},   // Type 4
    {5,  0, true},   // Type 5
    {6,  0, true},   // Type 6
    {7,  0, true},   // Type 7
    {8,  0, true},   // Type 8
    {9,  0, true},   // Type 9
    {10, 0, true},   // Type 10
    {11, 0, true},   // Type 11
    {12, 0, true},   // Type 12
    {13, 0, true},   // Type 13
    {14, 0, true},   // Type 14
    {15, 0, true},   // Type 15
    
    {0,  1, true},   // Type 16
    {1,  1, true},   // Type 17
    {2,  1, true},   // Type 18
    {3,  1, true},   // Type 19
    {4,  1, true},   // Type 20
    {5,  1, true},   // Type 21
    {6,  1, true},   // Type 22
    {7,  1, true},   // Type 23
    {8,  1, true},   // Type 24
    {9,  1, true},   // Type 25
    {10, 1, true},   // Type 26
    {11, 1, true},   // Type 27
    {12, 1, true},   // Type 28
    {13, 1, true},   // Type 29
    {14, 1, true},   // Type 30
    {15, 1, true},   // Type 31
    
    {0,  2, true},   // Type 32
    {1,  2, true},   // Type 33
    {2,  2, true},   // Type 34
    {3,  2, true},   // Type 35
    {4,  2, true},   // Type 36
    {5,  2, true},   // Type 37
    {6,  2, true},   // Type 38
    {7,  2, true},   // Type 39
    {8,  2, true},   // Type 40
    {9,  2, true},   // Type 41
    {10, 2, true},   // Type 42
    {11, 2, true},   // Type 43
    {12, 2, true},   // Type 44
    {13, 2, true},   // Type 45
    {14, 2, true},   // Type 46
    {15, 2, true},   // Type 47
    
    {0,  3, false},  // Type 48
    {1,  3, false},  // Type 49
    {2,  3, true},   // Type 50
    {3,  3, true},   // Type 51
    {4,  3, true},   // Type 52
    {5,  3, false},  // Type 53
    {6,  3, false},  // Type 54
    {7,  3, true},   // Type 55
    {8,  3, true},   // Type 56
    {9,  3, true},   // Type 57
    {10, 3, true},   // Type 58
    {11, 3, true},   // Type 59
    {12, 3, true},   // Type 60
    {13, 3, true},   // Type 61
    {14, 3, true},   // Type 62
    {15, 3, true},   // Type 63
    
    {0,  4, false},  // Type 64
    {1,  4, false},  // Type 65
    {2,  4, false},  // Type 66
    {3,  4, true},   // Type 67
    {4,  4, true},   // Type 68
    {5,  4, false},  // Type 69
    {6,  4, false},  // Type 70
    {7,  4, true},   // Type 71
    {8,  4, true},   // Type 72
    {9,  4, true},   // Type 73
    {10, 4, true},   // Type 74
    {11, 4, true},   // Type 75
    {12, 4, true},   // Type 76
    {13, 4, true},   // Type 77
    {14, 4, true},   // Type 78
    {15, 4, true},   // Type 79
    
    {0,  5, false},  // Type 80
    {1,  5, false},  // Type 81
    {2,  5, false},   // Type 82
    {3,  5, false},   // Type 83
    {4,  5, false},   // Type 84
    {5,  5, false},  // Type 85
    {6,  5, false},  // Type 86
    {7,  5, false},   // Type 87
    {8,  5, false},   // Type 88
    {9,  5, false},   // Type 89
    {10, 5, false},   // Type 90
    {11, 5, false},   // Type 91
    {12, 5, false},   // Type 92
    {13, 5, false},   // Type 93
    {14, 5, false},   // Type 94
    {15, 5, false},   // Type 95
    
    {0,  6, false},  // Type 96
    {1,  6, false},  // Type 97
    {2,  6, false},   // Type 98
    {3,  6, false},   // Type 99
    {4,  6, false},   // Type 100
    {5,  6, false},   // Type 101
    {6,  6, false},   // Type 102
    {7,  6, false},   // Type 103
    {8,  6, false},   // Type 104
    {9,  6, false},   // Type 105
    {10, 6, false},   // Type 106
    {11, 6, false},   // Type 107
    {12, 6, false},   // Type 108
    {13, 6, false},   // Type 109
    {14, 6, false},   // Type 110
    {15, 6, false},   // Type 111
    
    {0,  7, false},  // Type 112
    {1,  7, false},  // Type 113
    {2,  7, false},   // Type 114
    {3,  7, false},   // Type 115
    {4,  7, false},   // Type 116
    {5,  7, false},   // Type 117
    {6,  7, false},   // Type 118
    {7,  7, false},   // Type 119
    {8,  7, false},   // Type 120
    {9,  7, false},   // Type 121
    {10, 7, false},   // Type 122
    {11, 7, false},   // Type 123
    {12, 7, false},   // Type 124
    {13, 7, false},   // Type 125
    {14, 7, false},   // Type 126
    {15, 7, false},   // Type 127
    
    {0,  8, false},   // Type 128
    {1,  8, false},   // Type 129
    {2,  8, false},    // Type 130
    {3,  8, false},    // Type 131
    {4,  8, false},    // Type 132
    {5,  8, false},    // Type 133
    {6,  8, false},    // Type 134
    {7,  8, false},    // Type 135
    {8,  8, false},    // Type 136
    {9,  8, false},    // Type 137
    {10, 8, false},    // Type 138
    {11, 8, false},    // Type 139
    {12, 8, false},    // Type 140
    {13, 8, false},    // Type 141
    {14, 8, false},    // Type 142
    {15, 8, false},    // Type 143
    
    {0,  9, false},    // Type 144
    {1,  9, false},    // Type 145
    {2,  9, false},    // Type 146
    {3,  9, false},    // Type 147
    {4,  9, false},    // Type 148
    {5,  9, false},    // Type 149
    {6,  9, false},    // Type 150
    {7,  9, false},    // Type 151
    {8,  9, false},    // Type 152
    {9,  9, false},    // Type 153
    {10, 9, false},    // Type 154
    {11, 9, false},    // Type 155
    {12, 9, false},    // Type 156
    {13, 9, false},    // Type 157
    {14, 9, false},    // Type 158
    {15, 9, false},    // Type 159
    
    {0,  10, false},   // Type 160
    {1,  10, false},   // Type 161
    {2,  10, false},   // Type 162
    {3,  10, false},   // Type 163
    {4,  10, false},   // Type 164
    {5,  10, false},   // Type 165
    {6,  10, false},   // Type 166
    {7,  10, false},   // Type 167
    {8,  10, false},   // Type 168
    {9,  10, false},   // Type 169
    {10, 10, false},   // Type 170
    {11, 10, false},   // Type 171
    {12, 10, false},   // Type 172
    {13, 10, false},   // Type 173
    {14, 10, false},   // Type 174
    {15, 10, false},   // Type 175
    
    {0,  11, false},   // Type 176
    {1,  11, false},   // Type 177
    {2,  11, false},   // Type 178
    {3,  11, false},   // Type 179
    {4,  11, false},   // Type 180
    {5,  11, false},   // Type 181
    {6,  11, false},   // Type 182
    {7,  11, false},   // Type 183
    {8,  11, false},   // Type 184
    {9,  11, false},   // Type 185
    {10, 11, false},   // Type 186
    {11, 11, false},   // Type 187
    {12, 11, false},   // Type 188
    {13, 11, false},   // Type 189
    {14, 11, false},   // Type 190
    {15, 11, false},   // Type 191
    
    {0,  12, false},   // Type 192
    {1,  12, false},   // Type 193
    {2,  12, false},   // Type 194
    {3,  12, false},   // Type 195
    {4,  12, false},   // Type 196
    {5,  12, false},   // Type 197
    {6,  12, false},   // Type 198
    {7,  12, false},   // Type 199
    {8,  12, false},   // Type 200
    {9,  12, false},   // Type 201
    {10, 12, false},   // Type 202
    {11, 12, false},   // Type 203
    {12, 12, false},   // Type 204
    {13, 12, false},   // Type 205
    {14, 12, false},   // Type 206
    {15, 12, false},   // Type 207
    
    {0,  13, false},   // Type 208
    {1,  13, false},   // Type 209
    {2,  13, false},   // Type 210
    {3,  13, false},   // Type 211
    {4,  13, false},   // Type 212
    {5,  13, false},   // Type 213
    {6,  13, false},   // Type 214
    {7,  13, false},   // Type 215
    {8,  13, false},   // Type 216
    {9,  13, false},   // Type 217
    {10, 13, false},   // Type 218
    {11, 13, false},   // Type 219
    {12, 13, false},   // Type 220
    {13, 13, false},   // Type 221
    {14, 13, false},   // Type 222
    {15, 13, false},   // Type 223
    
    {0,  14, false},   // Type 224
    {1,  14, false},   // Type 225
    {2,  14, false},   // Type 226
    {3,  14, false},   // Type 227
    {4,  14, false},   // Type 228
    {5,  14, false},   // Type 229
    {6,  14, false},   // Type 230
    {7,  14, false},   // Type 231
    {8,  14, false},   // Type 232
    {9,  14, false},   // Type 233
    {10, 14, false},   // Type 234
    {11, 14, false},   // Type 235
    {12, 14, false},   // Type 236
    {13, 14, false},   // Type 237
    {14, 14, false},   // Type 238
    {15, 14, false},   // Type 239
    
    {0,  15, false},   // Type 240
    {1,  15, false},   // Type 241
    {2,  15, false},   // Type 242
    {3,  15, false},   // Type 243
    {4,  15, false},   // Type 244
    {5,  15, false},   // Type 245
    {6,  15, false},   // Type 246
    {7,  15, false},   // Type 247
    {8,  15, false},   // Type 248
    {9,  15, false},   // Type 249
    {10, 15, false},   // Type 250
    {11, 15, false},   // Type 251
    {12, 15, false},   // Type 252
    {13, 15, false},   // Type 253
    {14, 15, false},   // Type 254
    {15, 15, false},   // Type 255
};
