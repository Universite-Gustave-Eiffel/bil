* COPYRIGHT (c) 1993 Council for the Central Laboratory
*                    of the Research Councils
*######DATE 21 Dec 1992
C       Toolpack tool decs employed.
C
C  EAT 21/6/93 EXTERNAL statement put in for block data on VAXs.
C  JAS 29/4/98 Bug corrected. If (abs(IND).eq.1) IW must be length
C              MAX(NC,NR)+1
C
C
      SUBROUTINE MC49AD(IND,NC,NR,NNZ,IRN,JCN,YESA,LA,A,LIP,IP,LIW,IW,
     +                  IFLAG)
C
C   TO SORT THE PATTERN OF A SPARSE MATRIX FROM ARBITARY ORDER TO
C   COLUMN ORDER. AN OPTION EXISTS FOR ORDERING BY ROWS WITHIN EACH
C   COLUMN.
C
C   Argument list. * indicates changed on exit.
C   IND - INTEGER VARIABLE
C       - MUST BE SET TO -1,-2, 2, OR 1 ON ENTRY
C       - IF IND=1 or -1 THE ORDERING WITHIN EACH COLUMN WILL BE
C         ARBITRARY
C       - IF IND=2 or -2 THE ORDERING WITHIN EACH COLUMN WILL BE BY ROWS
C       - IF IND = 1 0R 2 THE USER-SUPPLIED DATA IS CHECKED.
C   NC  - INTEGER VARIABLE
C       - MUST BE SET TO THE NUMBER OF COLUMNS IN THE MATRIX
C       - RESTRICTION: NC.GE.1
C   NR  - INTEGER VARIABLE
C       - MUST BE SET TO THE NUMBER OF ROWS IN THE MATRIX
C       - RESTRICTION: NR.GE.1
C   NNZ - INTEGER VARIABLE
C       - MUST BE SET TO THE NUMBER OF NONZEROS IN THE MATRIX
C       - RESTRICTION: NNZ.GE.1
C  *IRN - INTEGER ARRAY OF LENGTH NNZ
C       - ON ENTRY SET TO CONTAIN THE ROW INDICES OF THE NONZEROS
C         IN ARBITARY ORDER.
C       - ON EXIT, IRN IS REORDERED SO THAT THE ROW INDICES
C         FOR COLUMN 1 PRECEDE THOSE FOR COLUMN 2 AND SO ON.
C       - IF IND=0 THE ORDER WITHIN COLUMNS IS ARBITRARY.
C       - IF IND=1 THE ORDER WITHIN COLUMNS IS BY ROWS.
C  *JCN - INTEGER ARRAY OF LENGTH NNZ
C       - ON ENTRY SET TO CONTAIN THE COLUMN INDICES OF THE NONZEROS
C       - JCN(K) MUST BE THE COLUMN INDEX OF
C         THE ENTRY IN IRN(K)
C  YESA - LOGICAL VARIABLE
C       - IF YESA IS SET TO .FALSE., THE ARRAY A IS NOT ACCESSED.
C   LA  - INTEGER VARIABLE WHICH MUST BE SET TO THE LENGTH OF THE
C         ARRAY A. IF YESA=.TRUE. LA MUST BE AT LEAST NNZ, AND
C         IF YESA=.FALSE. LA MUST BE AT LEAST 1.
C  *A   - REAL (DOUBLE PRECISION) ARRAY OF LENGTH LA
C       - IF YESA IS .TRUE., A(K)
C         MUST BE SET TO THE VALUE OF THE ENTRY IN (IRN(K),JCN(K))
C         AND ON EXIT, THE ARRAY WILL HAVE BEEN PERMUTED IN THE SAME
C         WAY AS IRN
C       - IF YESA IS .FALSE. THE ARRAY IS NOT ACCESSED
C   LIP - INTEGER VARIABLE WHICH DEFINES THE LENGTH OF THE ARRAY IP.
C       - IF IND=1 OR -1 LIP MUST BE AT LEAST NC+1
C       - IF IND=2 OR -2 LIP MUST BE AT LEAST MAX(NR,NC)+1
C  *IP  - INTEGER ARRAY OF LENGTH LIP
C       - NOT SET ON ENTRY
C       - ON EXIT, IP(J) CONTAINS THE POSITION IN IRN (AND A) OF THE
C         FIRST ENTRY IN COLUMN J (J=1,...,NC)
C         AND IP(NC+1) IS SET TO NNZ+1
C  LIW  - INTEGER VARIABLE WHICH DEFINES THE LENGTH OF THE ARRAY IW
C       - IF IND=1 OR -1 LIW MUST BE AT LEAST MAX(NR,NC)+1
C       - IF IND=2 OR -2 LIW MUST BE AT LEAST NR+1
C *IW   - INTEGER ARRAY OF LENGTH LIW
C       - THE ARRAY IS USED AS WORKSPACE
C *IFLAG - INTEGER VARIABLE. NOT SET ON ENTRY.
C          IF IND = 1 OR 2 ON SUCCESSFUL EXIT
C          IFLAG=0 AND OTHER VALUES OF IFLAG INDICATE AN ERROR.
C
C     .. Scalar Arguments ..
      INTEGER IFLAG,IND,LA,LIP,LIW,NC,NNZ,NR
      LOGICAL YESA
C     ..
C     .. Array Arguments ..
      DOUBLE PRECISION A(LA)
      INTEGER IP(LIP),IRN(NNZ),IW(LIW),JCN(NNZ)
C     ..
C     .. Local Scalars ..
      INTEGER I,J,K,KSTART,KSTOP,NZJ
C     ..
C     .. External Subroutines ..
      EXTERNAL MC49BD,MC49CD
C     ..
C     .. Data block external statement
      EXTERNAL MC49DD
C     ..
C     .. Intrinsic Functions ..
      INTRINSIC ABS,MAX
C     ..
C     .. Common blocks ..
      COMMON /MC49ED/LP,MP,IOUT,JOUT,IDUP,NZOUT
      INTEGER IDUP,IOUT,JOUT,LP,MP,NZOUT
C     ..
C     .. Save statement ..
      SAVE /MC49ED/
C     ..
C     .. Executable Statements ..
      IFLAG = 0
      NZOUT = 0
      IOUT = 0
      JOUT = 0
      IDUP = 0
C  CHECK THE INPUT DATA
      IF (IND.GT.2 .OR. IND.LT.-2 .OR. IND.EQ.0) THEN
        IFLAG = -1
        IF (LP.GT.0) THEN
          WRITE (LP,FMT=9000) IFLAG
          WRITE (LP,FMT=9010) IND
        END IF

        GO TO 70

      END IF

      IF (NC.LT.1 .OR. NR.LT.1 .OR. NNZ.LT.1) THEN
        IFLAG = -2
        IF (LP.GT.0) THEN
          WRITE (LP,FMT=9000) IFLAG
          WRITE (LP,FMT=9020)
          WRITE (LP,FMT=9030) NC,NR,NNZ
        END IF

        GO TO 70

      END IF

      IF (YESA) THEN
        IF (LA.LT.NNZ) THEN
          IFLAG = -3
          IF (LP.GT.0) THEN
            WRITE (LP,FMT=9000) IFLAG
            WRITE (LP,FMT=9040) LA,NNZ
          END IF

          GO TO 70

        END IF

      ELSE
        IF (LA.LT.1) THEN
          IFLAG = -3
          IF (LP.GT.0) THEN
            WRITE (LP,FMT=9000) IFLAG
            WRITE (LP,FMT=9050) LA
          END IF

          GO TO 70

        END IF

      END IF

      IF (ABS(IND).EQ.1 .AND. LIW.LT.MAX(NR,NC)+1) THEN
        IFLAG = -4
        IF (LP.GT.0) THEN
          WRITE (LP,FMT=9000) IFLAG
          WRITE (LP,FMT=9060) LIW,MAX(NR,NC) + 1
        END IF

        GO TO 70

      ELSE IF (ABS(IND).EQ.2 .AND. LIW.LT.NR+1) THEN
        IFLAG = -4
        IF (LP.GT.0) THEN
          WRITE (LP,FMT=9000) IFLAG
          WRITE (LP,FMT=9060) LIW,NR + 1
        END IF

        GO TO 70

      END IF

      IF (ABS(IND).EQ.1 .AND. LIP.LT.NC+1) THEN
        IFLAG = -5
        IF (LP.GT.0) THEN
          WRITE (LP,FMT=9000) IFLAG
          WRITE (LP,FMT=9070) LIP,NC + 1
        END IF

        GO TO 70

      ELSE IF (ABS(IND).EQ.2 .AND. LIP.LT.MAX(NR,NC)+1) THEN
        IFLAG = -5
        IF (LP.GT.0) THEN
          WRITE (LP,FMT=9000) IFLAG
          WRITE (LP,FMT=9070) LIP,MAX(NR,NC) + 1
        END IF

        GO TO 70

      END IF

      IF (IND.LT.0) THEN
        NZOUT = NNZ
        GO TO 20

      END IF

      DO 10 K = 1,NNZ
        I = IRN(K)
        J = JCN(K)
        IF (I.GT.NR .OR. I.LT.1) THEN
          IOUT = IOUT + 1

        ELSE IF (J.GT.NC .OR. J.LT.1) THEN
          JOUT = JOUT + 1

        ELSE
          NZOUT = NZOUT + 1
          IRN(NZOUT) = I
          JCN(NZOUT) = J
          IF (YESA) A(NZOUT) = A(K)
        END IF

   10 CONTINUE
      IF (IOUT.GT.0) THEN
        IFLAG = 2
        IF (MP.GT.0) THEN
          WRITE (MP,FMT=9080) IFLAG
          WRITE (MP,FMT=9090) IOUT
        END IF

      END IF

      IF (JOUT.GT.0) THEN
        IFLAG = 3
        IF (MP.GT.0) THEN
          WRITE (MP,FMT=9080) IFLAG
          WRITE (MP,FMT=9110) JOUT
        END IF

      END IF

      IF (IOUT+JOUT.EQ.NNZ) THEN
        NZOUT = 0
        GO TO 70

      END IF

   20 CONTINUE
C
C  NZOUT IS NOW THE NUMBER OF NONZERO ENTRIES
C  IF ABS(IND)=1 ORDER DIRECTLY BY COLUMNS
      IF (ABS(IND).EQ.1) THEN
        CALL MC49BD(NC,NZOUT,IRN,JCN,YESA,LA,A,IP,IW)

      ELSE
C
C IF ABS(IND)=2 ORDER FIRST ORDER BY ROWS
C
        CALL MC49BD(NR,NZOUT,JCN,IRN,YESA,LA,A,IW,IP)
C
C  NOW ORDER BY COLUMNS AND BY ROWS WITHIN EACH COLUMN
C
        CALL MC49CD(NC,NR,NZOUT,IRN,JCN,YESA,LA,A,IP,IW)
      END IF

      IF (IND.GT.0) THEN
C CHECK FOR DUPLICATES
        NZOUT = 0
        KSTART = 1
        NZJ = 0
        DO 30 I = 1,NR
          IW(I) = 0
   30   CONTINUE
        DO 50 J = 1,NC
          KSTOP = IP(J+1) - 1
          IP(J+1) = IP(J)
          DO 40 K = KSTART,KSTOP
            I = IRN(K)
            IF (IW(I).LE.NZJ) THEN
              NZOUT = NZOUT + 1
              IRN(NZOUT) = I
              IF (YESA) A(NZOUT) = A(K)
              IP(J+1) = IP(J+1) + 1
              IW(I) = NZOUT

            ELSE
C  WE HAVE A DUPLICATE IN COLUMN J
              IDUP = IDUP + 1
              IF (YESA) A(IW(I)) = A(IW(I)) + A(K)
            END IF

   40     CONTINUE
          KSTART = KSTOP + 1
          NZJ = NZOUT
   50   CONTINUE
        IF (IDUP.GT.0) THEN
          IFLAG = 1
          IF (MP.GT.0) THEN
            WRITE (MP,FMT=9080) IFLAG
            WRITE (MP,FMT=9100) IDUP
          END IF

        END IF

      END IF

   70 RETURN

 9000 FORMAT (/,' *** ERROR RETURN FROM MC49A/AD *** IFLAG = ',I2)
 9010 FORMAT (1X,'IND=',I2,' IS OUT OF RANGE')
 9020 FORMAT (1X,'NC, NR, OR, NNZ IS OUT OF RANGE')
 9030 FORMAT (1X,'NC=',I6,' NR=',I6,' NNZ=',I10)
 9040 FORMAT (1X,'INCREASE LA FROM',I8,' TO AT LEAST ',I8)
 9050 FORMAT (1X,'INCREASE LA FROM',I8,' TO AT LEAST 1')
 9060 FORMAT (1X,'INCREASE LIW FROM',I8,' TO AT LEAST ',I8)
 9070 FORMAT (1X,'INCREASE LIP FROM',I8,' TO AT LEAST ',I8)
 9080 FORMAT (/,' *** WARNING MESSAGE FROM MC49A/AD *** IFLAG = ',I2)
 9090 FORMAT (1X,I6,' ENTRIES IN IRN SUPPLIED BY THE USER WERE OUT OF ',
     +       /,'       RANGE AND WERE IGNORED BY THE ROUTINE')
 9100 FORMAT (1X,I6,' DUPLICATE ENTRIES WERE SUPPLIED BY THE USER')
 9110 FORMAT (1X,I6,' ENTRIES IN JCN SUPPLIED BY THE USER WERE OUT OF ',
     +       /,'       RANGE AND WERE IGNORED BY THE ROUTINE')

      END
C***********************************************************************
      SUBROUTINE MC49BD(NC,NNZ,IRN,JCN,YESA,LA,A,IP,IW)
C
C   TO SORT THE PATTERN OF A SPARSE MATRIX FROM ARBITARY ORDER TO
C   COLUMN ORDER, UNORDERED WITHIN EACH COLUMN.
C
C   Argument list. * indicates changed on exit.
C
C   NC - INTEGER VARIABLE
C      - ON ENTRY MUST BE SET TO THE NUMBER OF COLUMNS IN THE MATRIX
C  NNZ - INTEGER VARIABLE
C      - ON ENTRY, MUST BE SET TO THE NUMBER OF NONZEROS IN THE MATRIX
C *IRN - INTEGER ARRAY OF LENGTH NNZ
C      - ON ENTRY SET TO CONTAIN THE ROW INDICES OF THE NONZEROS
C        IN ARBITARY ORDER.
C      - ON EXIT, THE ENTRIES IN IRN ARE REORDERED SO THAT THE ROW
C        INDICES FOR COLUMN 1 PRECEDE THOSE FOR COLUMN 2 AND SO ON,
C        BUT THE ORDER WITHIN COLUMNS IS ARBITRARY.
C *JCN - INTEGER ARRAY OF LENGTH NNZ
C      - ON ENTRY SET TO CONTAIN THE COLUMN INDICES OF THE NONZEROS
C      - JCN(K) MUST BE THE COLUMN INDEX OF
C        THE ENTRY IN IRN(K)
C      - ON EXIT, JCN(K) IS THE COLUMN INDEX FOR THE ENTRY WITH
C        ROW INDEX IRN(K) (K=1,...,NNZ).
C YESA - LOGICAL VARIABLE
C      - IF YESA IS SET TO .FALSE., THE ARRAY A IS NOT ACCESSED.
C  LA  - INTEGER VARIABLE WHICH DEFINES THE LENGTH OF THE ARRAY A
C   *A - REAL (DOUBLE PRECISION) ARRAY OF LENGTH LA
C      - IF YESA IS .TRUE., THE ARRAY MUST BE OF LENGTH NNZ, AND A(K)
C        MUST BE SET TO THE VALUE OF THE ENTRY IN (IRN(K), JCN(K));
C        ON EXIT A IS REORDERED IN THE SAME WAY AS IRN
C      - IF YESA IS .FALSE. THE ARRAY IS NOT ACCESSED
C *IP  - INTEGER ARRAY OF LENGTH NC+1
C      - NOT SET ON ENTRY
C      - ON EXIT, IP(J) CONTAINS THE POSITION IN IRN (AND A) OF THE
C       FIRST ENTRY IN COLUMN J (J=1,...,NC)
C      - IP(NC+1) IS SET TO NNZ+1
C  *IW - INTEGER ARRAY OF LENGTH NC+1
C      - THE ARRAY IS USED AS WORKSPACE
C      - ON EXIT IW(I)=IP(I) (SO IW(I) POINTS TO THE BEGINNING
C        OF COLUMN I).
C     .. Scalar Arguments ..
      INTEGER LA,NC,NNZ
      LOGICAL YESA
C     ..
C     .. Array Arguments ..
      DOUBLE PRECISION A(LA)
      INTEGER IP(NC+1),IRN(NNZ),IW(NC+1),JCN(NNZ)
C     ..
C     .. Local Scalars ..
      DOUBLE PRECISION ACE,ACEP
      INTEGER ICE,ICEP,J,JCE,JCEP,K,L,LOC
C     ..
C     .. Executable Statements ..
      DO 10 J = 1,NC + 1
        IW(J) = 0
   10 CONTINUE
C**      COUNT THE NUMBER OF ELEMENTS IN EACH COLUMN AND STORE IN IW.
      DO 20 K = 1,NNZ
        J = JCN(K)
        IW(J) = IW(J) + 1
   20 CONTINUE
C
C**      PUT INTO IP AND IW THE POSITIONS WHERE EACH COLUMN
C        WOULD BEGIN IN A COMPRESSED COLLECTION WITH THE COLUMNS
C        IN NATURAL ORDER.
C
      IP(1) = 1
      DO 30 J = 2,NC + 1
        IP(J) = IW(J-1) + IP(J-1)
        IW(J-1) = IP(J-1)
   30 CONTINUE
C******  REORDER THE ELEMENTS INTO COLUMN ORDER.
C   FILL IN EACH COLUMN FROM THE FRONT, AND AS A NEW ENTRY IS PLACED
C   IN COLUMN K INCREASE THE POINTER IW(K) BY ONE.
C
      IF (YESA) GO TO 80
      DO 70 L = 1,NC
        DO 60 K = IW(L),IP(L+1) - 1
          ICE = IRN(K)
          JCE = JCN(K)
          DO 40 J = 1,NNZ
            IF (JCE.EQ.L) GO TO 50
            LOC = IW(JCE)
            JCEP = JCN(LOC)
            ICEP = IRN(LOC)
            IW(JCE) = LOC + 1
            JCN(LOC) = JCE
            IRN(LOC) = ICE
            JCE = JCEP
            ICE = ICEP
   40     CONTINUE
   50     JCN(K) = JCE
          IRN(K) = ICE
   60   CONTINUE
   70 CONTINUE
      GO TO 130

   80 CONTINUE
      DO 120 L = 1,NC
        DO 110 K = IW(L),IP(L+1) - 1
          ICE = IRN(K)
          JCE = JCN(K)
          ACE = A(K)
          DO 90 J = 1,NNZ
            IF (JCE.EQ.L) GO TO 100
            LOC = IW(JCE)
            JCEP = JCN(LOC)
            ICEP = IRN(LOC)
            IW(JCE) = LOC + 1
            JCN(LOC) = JCE
            IRN(LOC) = ICE
            JCE = JCEP
            ICE = ICEP
            ACEP = A(LOC)
            A(LOC) = ACE
            ACE = ACEP
   90     CONTINUE
  100     JCN(K) = JCE
          IRN(K) = ICE
          A(K) = ACE
  110   CONTINUE
  120 CONTINUE
  130 CONTINUE
      RETURN

      END
C
C**********************************************************
      SUBROUTINE MC49CD(NC,NR,NNZ,IRN,JCN,YESA,LA,A,IP,IW)
C
C   TO SORT THE ENTRIES OF A SPARSE MATRIX STORED BY ROWS,
C   UNORDERED WITHIN EACH ROW, TO ORDERING BY COLUMNS ,WITH
C   ORDERING BY ROWS WITHIN EACH COLUMN.
C
C   Argument list. * indicates changed on exit.
C
C   NC - INTEGER VARIABLE
C      - ON ENTRY MUST BE SET TO THE NUMBER OF COLUMNS IN THE MATRIX
C   NR - INTEGER VARIABLE
C      - ON ENTRY MUST BE SET TO THE NUMBER OF ROWS IN THE MATRIX
C  NNZ - INTEGER VARIABLE
C      - ON ENTRY MUST BE SET TO THE NUMBER OF NONZEROS IN THE MATRIX
C *IRN - INTEGER ARRAY OF LENGTH NNZ
C      - NOT SET ON ENTRY.
C      - ON EXIT IRN(K) CONTAINS THE ROW INDEX OF THE K TH ENTRY.
C      - THE ENTRIES ARE ORDERED SO THAT THE ENTRIES IN COLUMN 1
C        PRECEDE THOSE IN COLUMN 2,... AND THE ORDER WITHIN COLUMNS
C        IS BY ROWS.
C *JCN - INTEGER ARRAY OF LENGTH NNZ
C      - ON ENTRY SET TO CONTAIN THE COLUMN INDICES OF THE NONZEROS
C        ORDERED BY ROWS.
C      - THE CONTENTS OF THIS ARRAY ARE DESTROYED
C YESA - LOGICAL VARIABLE
C      - IF YESA IS SET TO .FALSE., THE ARRAY A IS NOT ACCESSED.
C  LA  - INTEGER VARIABLE WHICH DEFINES THE LENGTH OF THE ARRAY A
C  *A  - REAL (DOUBLE PRECISION) ARRAY OF LENGTH LA
C      - IF YESA IS .TRUE., THE ARRAY MUST BE OF LENGTH NNZ, AND ON
C        ENTRY A(K) MUST CONTAIN THE VALUE OF THE ENTRY IN JCN(K).
C      - ON EXIT, A IS REORDERED IN THE SAME WAY AS IRN
C  *IP - INTEGER ARRAY OF LENGTH NC+1
C      - NOT SET ON ENTRY.
C      - ON EXIT IP(I) POINTS TO THE FIRST ENTRY IN COLUMN I(I=1,...,NC)
C        AND IP(NC+1) IS SET TO NNZ+1
C   IW - INTEGER ARRAY OF LENGTH NR+1
C      - ON ENTRY, MUST BE SET ON ENTRY SO THAT IW(J) POINTS TO THE
C        POSITION IN JCN OF THE FIRST ENTRY IN ROW J ,J=1,..NR, AND
C        IW(NR+1) MUST BE SET TO NNZ+1
C
C     .. Scalar Arguments ..
      INTEGER LA,NC,NNZ,NR
      LOGICAL YESA
C     ..
C     .. Array Arguments ..
      DOUBLE PRECISION A(LA)
      INTEGER IP(NC+1),IRN(NNZ),IW(NR+1),JCN(NNZ)
C     ..
C     .. Local Scalars ..
      DOUBLE PRECISION ACE,ACEP
      INTEGER I,ICE,ICEP,J,K,L,LOC,LOCP
C     ..
C     .. Executable Statements ..
      DO 10 J = 1,NC
        IP(J) = 0
   10 CONTINUE
      IF (.NOT.YESA) GO TO 80
C
C  COUNT THE NUMBER OF ENTRIES IN EACH COLUMN
C
      DO 20 K = 1,NNZ
        I = JCN(K)
        IP(I) = IP(I) + 1
        IRN(K) = JCN(K)
   20 CONTINUE
      IP(NC+1) = NNZ + 1
C
C  SET IP SO THE IP(I) POINTS TO THE FIRST ENTRY IN COLUMN I+1
C
      IP(1) = IP(1) + 1
      DO 30 J = 2,NC
        IP(J) = IP(J) + IP(J-1)
   30 CONTINUE
C
      DO 50 I = NR,1,-1
        DO 40 J = IW(I),IW(I+1) - 1
          K = IRN(J)
          L = IP(K) - 1
          JCN(J) = L
          IP(K) = L
          IRN(J) = I
   40   CONTINUE
   50 CONTINUE
      IP(NC+1) = NNZ + 1
      DO 70 J = 1,NNZ
        LOC = JCN(J)
        IF (LOC.EQ.0) GO TO 70
        ICE = IRN(J)
        ACE = A(J)
        JCN(J) = 0
        DO 60 K = 1,NNZ
          LOCP = JCN(LOC)
          ICEP = IRN(LOC)
          ACEP = A(LOC)
          JCN(LOC) = 0
          IRN(LOC) = ICE
          A(LOC) = ACE
          IF (LOCP.EQ.0) GO TO 70
          ICE = ICEP
          ACE = ACEP
          LOC = LOCP
   60   CONTINUE
   70 CONTINUE
      GO TO 130

   80 CONTINUE
C
C  COUNT THE NUMBER OF ENTRIES IN EACH COLUMN
C
      DO 90 K = 1,NNZ
        I = JCN(K)
        IP(I) = IP(I) + 1
   90 CONTINUE
      IP(NC+1) = NNZ + 1
C
C  SET IP SO THE IP(I) POINTS TO THE FIRST ENTRY IN COLUMN I+1
C
      IP(1) = IP(1) + 1
      DO 100 J = 2,NC
        IP(J) = IP(J) + IP(J-1)
  100 CONTINUE
C
      DO 120 I = NR,1,-1
        DO 110 J = IW(I),IW(I+1) - 1
          K = JCN(J)
          L = IP(K) - 1
          IP(K) = L
          IRN(L) = I
  110   CONTINUE
  120 CONTINUE
  130 RETURN

      END
C**********************************************************************
      BLOCK DATA MC49DD
C     .. Common blocks ..
      COMMON /MC49ED/LP,MP,IOUT,JOUT,IDUP,NZOUT
      INTEGER IDUP,IOUT,JOUT,LP,MP,NZOUT
C     ..
C     .. Save statement ..
      SAVE /MC49ED/
C     ..
C     .. Data statements ..
      DATA LP/6/,MP/6/
C     ..
C     .. Executable Statements ..
      END
