function rotateRight(n,x) {
    return ((x >>> n) | (x << (32 - n)));
}
function choice(x,y,z) {
    return ((x & y) ^ (~x & z));
}
function majority(x,y,z) {
    return ((x & y) ^ (x & z) ^ (y & z));
}
function sha256_Sigma0(x) {
    return (rotateRight(2, x) ^ rotateRight(13, x) ^ rotateRight(22, x));
}
function sha256_Sigma1(x) {
    return (rotateRight(6, x) ^ rotateRight(11, x) ^ rotateRight(25, x));
}
function sha256_sigma0(x) {
    return (rotateRight(7, x) ^ rotateRight(18, x) ^ (x >>> 3));
}
function sha256_sigma1(x) {
    return (rotateRight(17, x) ^ rotateRight(19, x) ^ (x >>> 10));
}
function sha256_expand(W, j) {
    return (W[j&0x0f] += sha256_sigma1(W[(j+14)&0x0f]) + W[(j+9)&0x0f] +
        sha256_sigma0(W[(j+1)&0x0f]));
}

const K256 = [0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2];

let iHash, count, buffer;
const sha256_hex_digits = "0123456789abcdef";

function safe_add(x, y) {
    const lsw = (x & 0xffff) + (y & 0xffff);
    const msw = (x >> 16) + (y >> 16) + (lsw >> 16);
    return (msw << 16) | (lsw & 0xffff);
}

function sha256_init() {
    iHash = new Array(8);
    count = new Array(2);
    buffer = new Array(64);
    count[0] = count[1] = 0;
    iHash[0] = 0x6a09e667;
    iHash[1] = 0xbb67ae85;
    iHash[2] = 0x3c6ef372;
    iHash[3] = 0xa54ff53a;
    iHash[4] = 0x510e527f;
    iHash[5] = 0x9b05688c;
    iHash[6] = 0x1f83d9ab;
    iHash[7] = 0x5be0cd19;
}

function sha256_transform() {
    let a, b, c, d, e, f, g, h, T1, T2;
    const W = new Array(16);

    a = iHash[0];
    b = iHash[1];
    c = iHash[2];
    d = iHash[3];
    e = iHash[4];
    f = iHash[5];
    g = iHash[6];
    h = iHash[7];

    for(let i=0; i<16; i++)
        W[i] = ((buffer[(i<<2)+3]) | (buffer[(i<<2)+2] << 8) | (buffer[(i<<2)+1]
            << 16) | (buffer[i<<2] << 24));

    for(let j=0; j<64; j++) {
        T1 = h + sha256_Sigma1(e) + choice(e, f, g) + K256[j];
        if(j < 16) T1 += W[j];
        else T1 += sha256_expand(W, j);
        T2 = sha256_Sigma0(a) + majority(a, b, c);
        h = g;
        g = f;
        f = e;
        e = safe_add(d, T1);
        d = c;
        c = b;
        b = a;
        a = safe_add(T1, T2);
    }

    iHash[0] += a;
    iHash[1] += b;
    iHash[2] += c;
    iHash[3] += d;
    iHash[4] += e;
    iHash[5] += f;
    iHash[6] += g;
    iHash[7] += h;
}

function sha256_update(data, inputLen) {
    let j;
    let i, index, corpus = 0;
    index = ((count[0] >> 3) & 0x3f);
    const remainder = (inputLen & 0x3f);
    if ((count[0] += (inputLen << 3)) < (inputLen << 3)) count[1]++;
    count[1] += (inputLen >> 29);
    for(i=0; i+63<inputLen; i+=64) {
        for(j = index; j<64; j++)
            buffer[j] = data.charCodeAt(corpus++);
        sha256_transform();
        index = 0;
    }
    for(j = 0; j<remainder; j++)
        buffer[j] = data.charCodeAt(corpus++);
}

function sha256_final() {
    let i;
    let index = ((count[0] >> 3) & 0x3f);
    buffer[index++] = 0x80;
    if(index <= 56) {
        for(i = index; i<56; i++)
            buffer[i] = 0;
    } else {
        for(i = index; i<64; i++)
            buffer[i] = 0;
        sha256_transform();
        for(i = 0; i<56; i++)
            buffer[i] = 0;
    }
    buffer[56] = (count[1] >>> 24) & 0xff;
    buffer[57] = (count[1] >>> 16) & 0xff;
    buffer[58] = (count[1] >>> 8) & 0xff;
    buffer[59] = count[1] & 0xff;
    buffer[60] = (count[0] >>> 24) & 0xff;
    buffer[61] = (count[0] >>> 16) & 0xff;
    buffer[62] = (count[0] >>> 8) & 0xff;
    buffer[63] = count[0] & 0xff;
    sha256_transform();
}

function sha256_encode_hex() {
    let output = String();
    for(let i=0; i<8; i++) {
        for(let j=28; j>=0; j-=4)
            output += sha256_hex_digits.charAt((iHash[i] >>> j) & 0x0f);
    }
    return output;
}

function sha256_digest(data) {
    sha256_init();
    sha256_update(data, data.length);
    sha256_final();
    return sha256_encode_hex();
}

function sha256_separation(sha256_data) {
    let temp_data=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
    for (let i=0;i<32;i++){
        temp_data[i]=parseInt(sha256_data.substring(2*i,2*i+2),16);
    }
    return temp_data;
}