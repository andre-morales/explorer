extern const char* const DEFAULT_CONFIG;

const char* const DEFAULT_CONFIG = R"(%YAML 1.1
Name: Explorer Server
MaxConnections: 20
Password: ""

Compression: 
    Enabled: true
    Methods:
        - raw

        - gzip.0
        - gzip.1
        - gzip.2
        - gzip.3
        - gzip.4
        - gzip.5
        - gzip.6
        - gzip.7
        - gzip.8
        - gzip.9

        - bzip2.1
        - bzip2.2
        - bzip2.3
        - bzip2.4
        - bzip2.5
        - bzip2.6
        - bzip2.7
        - bzip2.8
        - bzip2.9

Encryption:
    Enabled: false
    Methods:
        - raw
        
        - rsa_aes128
        - rsa_aes256

Universes:
    default:
        Name: Explorer Universe
        Planets:
            default:
                Name: Explorer Planet

Spawn: default, default, 0.0, 30.0, 0.0


)";
