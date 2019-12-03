# IMS669-secp256r1
IMS669 수업에서 다루는 secp256r1(P-256 커브 타원곡선)에 대한 코드를 정리합니다.

### 유의사항
* 가능한 한 32비트와 64비트 시스템 모두에서 동작하도록 작성하지만, 주된 테스트는 64비트 환경에서 진행됩니다.
* 일부 단계에서 GMP(GNU Multiple-Precision Library)를 사용해 테스트하는데, 이때 사용한 GMP가 64비트(기본 word-size가 8바이트)라는 점에서 문제가 발생할 수 있습니다.
* 본 코드의 Ctrl+C / Ctrl+V로 인해 발생할 수 있는 표절 관련 문제에 대해서는 책임을 지지 않습니다. 참고용으로만 사용해주세요



### 헤더파일 사이의 include 관계

test를 제외한 나머지 파일(`p256_config.h`, `p256_int.h`, `p256_AF.h`, `p256_ECC.h`) 사이의 관계는 다음과 같습니다.

![코드 간 include 관계](https://github.com/nyan101/IMS669-secp256r1/blob/master/include.png)

* `GMP` : 큰 수 연산을 위한 GNU MP 라이브러리. $\mathbb{F}_p$ 에서의 inverse 연산을 포함해 일부 영역에서 사용
* `p256_config` : 32비트와 64비트의 차이로 인해 변하는 상수를 모아둔 헤더
* `p256_int` : secp256r1에서 사용되는 prime $p= 2^256 - 2^224 + 2^192 + 2^96 - 1$ 에 대해$\mod p$, 다시 말해 $\mathbb{F}_p$ 위에서의 연산에 관련된 코드
* `p256_AF` : secp256r1에서 사용되는 타원곡선 $y^2=x^3+ax+b$  위에서의 point 연산에 관련된 코드
* `p256_ECC` : `p256_int`, `p256_AF`를 실질적으로 활용하는 부분. 본 코드에서는 ECDSA(Elliptic Curve Digital Signature Algorithm)을 구현