HOOK BOOT Ver.0.94

API���t�b�N���ă��j�^����A�p�b�`�����Ă�B
�J�[�l�����[�h�œ����A�ƂĂ��댯�ȃv���O�����Ȃ̂ŁA
���ȐӔC�Ŏg������

-- HOOK BOOT���ĂȂɁH

BOOT.BIN�Ƀp�b�`�����ĂāAAPI���t�b�N����B

�P�DAPI����ʂ�t�@�C���Ń��j�^�ł���B

�Q�DUMD�G�~�����[�V������MS����N���ł���B
�@�@�o�b�e���̃Z�[�u�ɂ�钷���ԃv���C�ƁA�����A�N�Z�X�ׂ̈ɁI

3.�C���|�[�g���C�u�����̏�񂪂݂��B�|�����

4.���s���A�J�[�l���ƃ��[�U�������Ƀp�b�`����B

-- ����

V0.94
   see readme.txt

V0.93
  -sceIoDread�Ńp�X��u�������Ȃ��������C��
  -�L���b�V���N���A����ǉ�
   (SYSCALL HOOK�ƃt�@�C�����O�Ńt���[�Y�̉���j
  -UMD�R�[���o�b�N�̃t�b�N��ύX�AsceCreateCallback�̃t�b�N�͕s�v
  -sceIoWrite�̃n���h��1�̏������݂����O�o��(SYSTEM LOG)
  -�t�b�NAPIsceIoSync and sceIoChstat�̒ǉ�
  -sceIoDevctl�Ƀp�X�u�������ǉ�

V0.92B
  -�T�C�Y�k��
  -CODED ARMS�t�B�N�X

V0.92A
  -KERNEL_VER parameter���ݒ莞�A�J�[�l���o�[�W�����p�b�`���o�C�p�X.
  -�t�b�N�֐��o�O�Ƃ�@sceIoLseek,sceIoLseekAsync,sceIoGetAsyncStat

V0.92
  -�C���|�[�g���C�u�����̃_�C�i�~�b�N�o�[�W�����t�b�N���T�|�[�g
�@�@������������1.00��1.50�\�t�g���N������B
�@�@'CODEDARMS/boot.ini'��"IMPORT_VER"�����Ȃ����B
  -"WAIT_RUN=[wait_mode]"�p�����[�^�̒ǉ�
  -sceIoLseek �� sceIoLseekAsync �̃t�b�N���A
  -sceKernelLoadExec �̃t�b�N�ǉ��i�ł��A�͂���j
V0.91
 readme.txt��"HISTRY"���Q��

V0.90
 readme.txt��"HISTRY"���Q��


V0.10 1st release

-- �Z�p

1.MODULE patch mode
�@1) ���[�h�O
�@�@�^�[�Q�b�g��NID��ׂ��āA�G���g���[�|�C���g�ɖ��ߍ���
�@2)���[�h��
  �@NID����G���g���[�|�C���g��T���ăt�b�N����B

2.SYSTEM CALL hook mode
  SYSTEMCALL�̃W�����v�x�N�^�[�e�[�u����ύX���ăt�b�N���܂��B

-- ����

�t�l�c�G�~�����[�V�����͊J�����ɂ��A�s���S

1.MODULE patch mode
  �Í�������Ă��Ȃ�ELF�̂݃T�|�[�g
�@�V���O�����W���[���̂ݑΉ��A�T�u���W���[���̓t�b�N�s��

2.SYSTEM CALL hook mode
  ���O�p�b�`�Ȃ��̒��ڋN����
�@�Í���~PSP file�T�|�[�g (EBOOT.BIN).
�@��������肠��A�isceCreateCallback�s���j

-- �g����

PuzzleBobble�̏ꍇ�A

1.MS_ROOT_DIR���̃t�@�C����MS�̃��[�g�ɃR�s�[
2.UMD�t�@�C����'/PSP/GAME/PBOBBLE/UMD0/'�ɃR�s�[
3.'/PSP/HOOKBOOT/EBOOT.PBP'��'/PSP/GAME/PBOBBLE/'�ɃR�s�[
  (Formware 1.50��PBP tool���g��)
4.'/PSP/GAME/PBOBBLE/EBOOT.PBP'�����s

�⑫�F
�@UMD�t�@�C���R�s�[��APC��MS�}�E���g����PSP/HOOKBOOT����
"makepbp [game�f�B���N�g��]"�����s����ƁA�Q�[���̃A�C�R���A
�w�i�A�T�E���h��g�ݍ���EBOOT.PBP������B
�@(1.50�̕��́A�������ԕK�v)

-- UMD�G�~�����[�V����

1.MS����KernelModule�̃��[�h���\�ɂ��܂��B
2.UMD�h���C�u�̏�Ԃ��G�~�����[�V�������܂��B

-- �����̌v��

1.FTP�N���C���g�ɂ��AUMD�G�~�����[�V����

2.���S�ȁA�o�[�W�����A�b�v�J�[�l���p�b�`
�@����́A�Ƃ肠�����A����

