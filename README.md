# decode_ASN1_BER
Dumps the contents of an ASN.1 BER file.
ASN.1 is a language that defines the way data is sent across
dissimilar communication systems. ASN.1 ensures that the data
received is the same as the data transmitted by providing a common
syntax for specifying application layer protocols. ASN.1 is an
ISO/ITU-T standard based on the OSI model and is defined in “ASN.1
encoding rules: Specification of Basic Notation, ITU-T
Recommendation X.680”.

BER (Basic Encoding Rule) is a type of
ASN.1 where the data is encoded in the following parts:

TAG: identifies the data type and consists of an ASN.1 tag
and data structure information

LENGTH: the length of the content part in number of octets

VALUE: the value encoded according to data type specific rules.

See tag_encoding.png and length_encoding.png files for details.

<hr>
Compile using any c++ compiler like 

g++ decoder.cpp -o decoder

It takes the BER filename as a parameter and prints the decoded output(TAG-LEN-VALUEinHex) to the stdout.

e.g: ./decoder ber_file
