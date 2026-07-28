#include "seal/seal.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace seal;


int main()
{
    // 1. 明文卷积

    vector<vector<int64_t>> input_matrix =
    {
        {1,2,3,4},
        {5,6,7,8},
        {9,10,11,12},
        {13,14,15,16}
    };

    vector<vector<int64_t>> kernel =
    {
        {1,0,1},
        {0,1,0},
        {1,0,1}
    };

    auto plain_convolution = [](const vector<vector<int64_t>> &input, const vector<vector<int64_t>> &kernel)
    {
        vector<vector<int64_t>> output(2, vector<int64_t>(2, 0));

        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                int64_t sum = 0;
                for(int x = 0; x < 3; x++)
                {
                    for(int y = 0; y < 3; y++)
                    {
                        sum += input[i+x][j+y] * kernel[x][y];
                    }
                }
                output[i][j] = sum;
            }
        }

        return output;
    };

    auto plain_result = plain_convolution(input_matrix, kernel);

    cout << "Plain convolution result:" << endl;

    for(auto row : plain_result)
    {
        for(auto x : row)
            cout << x << " ";
        cout << endl;
    }


    // 2. BFV 参数初始化

    EncryptionParameters parms(scheme_type::bfv);

    size_t poly_modulus_degree = 4096;

    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));

    SEALContext context(parms);


    // 3. 创建密钥和计算对象

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();

    PublicKey public_key;
    keygen.create_public_key(public_key);

    GaloisKeys galois_keys;
    keygen.create_galois_keys(galois_keys);

    Encryptor encryptor(context, public_key);
    Decryptor decryptor(context, secret_key);
    Evaluator evaluator(context);
    BatchEncoder encoder(context);


    // 4. 数据编码与加密

    vector<int64_t> input =
    {
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,15,16
    };

    Plaintext plain;
    encoder.encode(input, plain);

    Ciphertext encrypted;
    encryptor.encrypt(plain, encrypted);

    cout << "\nEncryption finished" << endl;


    // 5. 密文卷积
    // kernel:
    // 1 0 1
    // 0 1 0
    // 1 0 1
    // 对应 slot 偏移:
    // 0 2 5 8 10

    vector<int> rotations = {0,2,5,8,10};

    Ciphertext result;
    bool first = true;

    for(auto r : rotations)
    {
        Ciphertext temp;

        if(r == 0)
        {
            temp = encrypted;
        }

        else
        {
            evaluator.rotate_rows(encrypted, r, galois_keys, temp);
        }

        if(first)
        {
            result = temp;
            first = false;
        }

        else
        {
            evaluator.add_inplace(result, temp);
        }
    }


    // 6. Mask 提取有效输出

    vector<int64_t> mask =
    {
        1,1,0,0,
        1,1,0,0,
        0,0,0,0,
        0,0,0,0
    };

    Plaintext mask_plain;
    encoder.encode(mask, mask_plain);

    Ciphertext final_result;
    evaluator.multiply_plain(result, mask_plain, final_result);


    // 7. 解密密文卷积结果

    Plaintext decrypted;
    decryptor.decrypt(final_result, decrypted);

    vector<int64_t> output;
    encoder.decode(decrypted, output);

    cout << "\nDecrypted convolution result:" << endl;
    cout << output[0] << " " << output[1] << endl;
    cout << output[4] << " " << output[5] << endl;

    return 0;
}