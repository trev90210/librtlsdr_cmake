# Introduction
rtl_sdr IQ reader example

## Build instructions

### 1. Change directory
<code>cd examples</code>

### 2. Configure cmake
<code>cmake . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release</code>

### 3. Build files
<code>ninja -c build</code>

### 4. Run app
<code>build/rtl_sdr</code>