//===========================================================================
//
//  ��Ȩ�����ߣ� ���¹����㽭��ѧ�������ѧ�뼼��ѧԺ
//                       CAD&CG�����ص�ʵ����
//               xgliu@cad.zju.edu.cn
//  ����޸ģ�2019��2��28��
//            �����˿ؼ�����ɫ���������ã�
//            �Լ�������ɫ�����ӣ��ں���demoGuiALL.c��drawButtons�����
//  ����޸ģ�2019��2��26��
//            ��������ʾ�ı��༭��ʾ
//            �����˶�����ʾ
//            ������textbox �ı�����ؼ�
//            ���˲˵�����
//            �� uiGetInput Ϊ uiGetMouse,uiGetKey,uiGetKeyboard
//  ����޸ģ�2019��2��18��
//  ���δ�����2018��4�£�����<<�������ר��>>�γ̽�ѧ
//
//===========================================================================

#ifndef ____ui_button_h______
#define ____ui_button_h______

typedef struct
{
	double mousex;
	double mousey; // ?????????????
	int mousedown;
	int clickedItem;  // item that was clicked
	int actingMenu;	  // acting menu list
	int kbdItem;	  // item that takes keyboard
	int lastItem;	  // item that had focus just before
	int keyPress;	  // input key
	int charInput;	  // input char
	int keyModifiers; //  key modifier (shift = 1, ctrl = 2, shift+ctrl = 3)
} UIState;

UIState getUIState();

//===========================================================================
//
//   [L:16-1][N:16-1]
//        XOR
//   [F:32 --------1]
//
// Generate a *fake* unique ID for gui controls at compiling/run time
//
#define GenUIID(N) (((__LINE__ << 16) | (N & 0xFFFF)) ^ ((long)&__FILE__))
//
// GenUIID(0) will give a unique ID at each source code line.
// If you need one UI ID per line, just call GenUIID with 0
//
//               GenUIID(0)
//
// But, two GenUIID(0) calls at the same line will give the same ID.
//
// So, in a while/for loop body, GenUIID(0) will give you the same ID.
// In this case, you need call GenUIID with different N parameter:
//
//               GenUIID(N)
//
//===========================================================================

/* ��������	InitGUI
 *
 * ���ܣ���ʼ������
 *
 * �÷����ڴ��ڴ�������������֮�����
 */
void InitGUI();

/* ��������	uiGetMouse
 *			uiGetKeyboard
 *			uiGetChar
 *
 * ���ܣ�����û������/����/�ı�������
 *
 * �÷��������/����/�ı�����Ļص�������ʹ��
 *
 *		void MouseEventProcess(int x, int y, int button, int event)
 *		{
 *			uiGetMouse(x,y,button,event);
 *			...��������...
 *		}
 *
 *		void KeyboardEventProcess(int key, int event)
 *		{
 *			uiGetKeyboard(key,event);
 *			...��������...
 *		}
 *
 *		void CharEventProcess(char ch)
 *		{
 *			uiGetChar(ch);
 *			...��������...
 *		}
 */
void uiGetMouse(int x, int y, int button, int event);
void uiGetKeyboard(int key, int event);
void uiGetChar(int ch);

/*
 * ��������button
 *
 * ���ܣ���ʾһ����ť��button��
 *
 * �÷���if( button(GenUUID(0),x,y,w,h,label) ) {
 *           ִ����䣬��Ӧ�û����¸ð�ť
 *       }
 *
 *   id  - Ψһ��
 *   x,y - λ��
 *   w,h - ���Ⱥ͸߶�
 *   label - ��ť�ϵ����ֱ�ǩ
 *
 * ����ֵ
 *   0 - �û�û�е�������²��ͷţ���ť
 *   1 - ����˰�ť
 */
int button(int id, double x, double y, double w, double h, char *label);

/*
 * ��������menuList
 *
 * ���ܣ���ʾһ��˵�
 *
 * �÷���choice = menuList(GenUUID(0),x,y,w,h,labels,n);
 *
 *   id  - �˵���Ψһ��
 *   x,y - �˵���λ��
 *   w,h - �˵���Ĵ�С
 *   wlist - �˵��б��Ŀ���
 *   labels[] - ��ǩ���֣�[0]�ǲ˵����[1...n-1]�ǲ˵��б�
 *   n   - �˵���ĸ���
 *
 * ����ֵ
 *   -1    - �û�û�е�������²��ͷţ���ť
 *   >=0   - �û�ѡ�еĲ˵��� index ����labels[]�У�
 *
 * Ӧ�þ�����

	char * menuListFile[] = {"File",
		"Open  | Ctrl-O",  // ��ݼ��������[Ctrl-X]��ʽ�������ַ����Ľ�β
		"Close",
		"Exit   | Ctrl-E"};// ��ݼ��������[Ctrl-X]��ʽ�������ַ����Ľ�β

	int selection;

	selection = menuList(GenUIID(0), x, y, w, wlist, h, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));

	if( selection==3 )
		exit(-1); // user choose to exit

 */
int menuList(int id, double x, double y, double w, double wlist, double h, char *labels[], int n);
// �ò˵�����ɫ����һ�����Σ�λ��(x,y),����(w,h)
// һ�� w ���ڴ��ڵĿ��ȣ�h �Ͳ˵���ĸ߶�ƥ��
void drawMenuBar(double x, double y, double w, double h);

/*
 * ��������textbox
 *
 * ���ܣ���ʾһ���ı��༭����ʾ�ͱ༭�ı��ַ���
 *
 * �÷���textbox(GenUUID(0),x,y,w,h,textbuf,buflen);
 *       ����
		 if( textbox(GenUUID(0),x,y,w,h,textbuf,buflen) ) {
 *           �ı��ַ������༭�޸��ˣ�ִ����Ӧ���
 *       }
 *
 *   id  - Ψһ�ţ�һ����GenUUID(0), ����GenUUID��k)��k��ѭ��������
 *   x,y - �ı���λ��
 *   w,h - �ı���Ŀ��Ⱥ͸߶�
 *   textbuf - ���༭���ı��ַ�������\0��β��
 *   buflen - �ɴ洢���ı��ַ�������󳤶�
 * ����ֵ
 *   0 - �ı�û�б��༭
 *   1 - ���༭��
 */
int textbox(int id, double x, double y, double w, double h, char textbuf[], int buflen);

/*
 * ���ÿؼ�����ɫ
 *
 * �����͹���
 *      setButtonColors   - ���ð�ť��ɫ
 *      setMenuColors     - ���ò˵���ɫ
 *      setTextBoxColors  - ���ñ༭����ɫ
 * ����
 *      frame/label       - �ؼ���/���ֱ�ǩ����ɫ
 *      hotFrame/hotLabel - ��껮��ʱ���ؼ���/���ֱ�ǩ����ɫ
 *      fillflag          - �Ƿ���䱳����0 - ����䣬1 - ���
 * ˵��
 *      ��ĳ����ɫ�ַ���Ϊ��ʱ����Ӧ����ɫ��������
 */
void setButtonColors(char *frame, char *label, char *hotFrame, char *hotLabel, int fillflag);
void setMenuColors(char *frame, char *label, char *hotFrame, char *hotLabel, int fillflag);
void setTextBoxColors(char *frame, char *label, char *hotFrame, char *hotLabel, int fillflag);

// ʹ��Ԥ�������ɫ���
void usePredefinedColors(int k);
void usePredefinedButtonColors(int k);
void usePredefinedMenuColors(int k);
void usePredefinedTexBoxColors(int k);

/* ��ʾ�ַ�����ǩ */
void drawLabel(double x, double y, char *label);

/* ��һ������ */
void drawRectangle(double x, double y, double w, double h, int fillflag);

/* ��ʾ���ַ�����ǩ�ľ���
 *
 * xalignment - ��ǩ�;��εĶ��뷽ʽ
 *              'L' - ����
 *			    'R' - ����
 *              ����- ����
 *
 * fillflag - 1 ���
 *            0 �����
 */
void drawBox(double x, double y, double w, double h, int fillflag, char *label, char xalignment, char *labelColor);

#endif // define ____ui_button_h______
