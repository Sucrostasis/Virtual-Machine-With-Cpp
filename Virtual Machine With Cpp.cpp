#include<iostream>
#include<algorithm>
#include<vector>
using namespace std;

// 定义常量 
// Define constants
#define N 1<<20          // 内存大小（1MB） // Memory size (1MB)
#define CHECK_ADDR(x) if(x<0||x>=N){cerr<<"Error(line"<<i<<"): Invalid address\n";break;}  // 地址检查宏 // Address validation macro

int m[N],pos;           // 内存数组和临时位置变量 // Memory array and temporary position variable
string code[N]={        // 预定义指令集 // Predefined instruction set
    "input 0",
    "input 1",
    "calc add 0 1 2",
    "output int 2",
    "exit"
};

// 从代码行x的y位置读取字符串 
// Read string from code line x starting at position y
string readStr(int x,int y){
    pos=y;
    string ret;
    int len=code[x].size();
    while(pos<len&&code[x][pos]==' '){ // 跳过前导空格 // Skip leading spaces
		++pos;
	}
    while(pos<len&&code[x][pos]!=' '){ // 读取非空格字符 // Read non-space characters
		ret+=code[x][pos++];
	}
    return ret;
}

// 从代码行x的y位置读取整数 
// Read integer from code line x starting at position y
int readInt(int x,int y){
    pos=y;
    int ret=0,sign=1,len=code[x].size();
    while(pos<len&&(code[x][pos]<'0'||code[x][pos]>'9')){ // 跳过非数字字符 // Skip non-digit characters
        if(code[x][pos]=='-')sign=-1;  // 处理负号 // Handle negative sign
        ++pos;
    }
    while(pos<len&&isdigit(code[x][pos])){ // 读取数字部分 // Read numeric part
		ret=ret*10+(code[x][pos++]-'0');
	}
    return ret*sign;
}

int main(){
    int code_lines=0;
    // 计算有效代码行数 
	// Calculate valid code lines
    while(code_lines<N&&!code[code_lines].empty())++code_lines;
    
    // 主解释器循环 
	// Main interpreter loop
    for(int i=0;i<code_lines;){
        string op=readStr(i,0);  // 读取操作码 // Read operation code
        
        if(op=="input"){         // 输入指令 // Input instruction
            int x=readInt(i,pos);
            CHECK_ADDR(x);
            if(!(cin>>m[x])){    // 输入验证 // Input validation
				cerr<<"Error(line"<<i<<"): Input failure\n";
				break;
			}
            ++i;
        }else if(op=="output"){  // 输出指令 // Output instruction
            string mode=readStr(i,pos);
            int x=readInt(i,pos);
            CHECK_ADDR(x);
            if(mode=="int"){      // 整数模式输出 // Integer output mode
				cout<<m[x];
			}else if(mode=="char"){  // 字符模式输出 // Character output mode
				cout<<char(m[x]&0x7F); // 取低7位 // Use lower 7 bits
			}else{
				cerr<<"Error(line"<<i<<"): Invalid output mode\n";
				break;
			}
            ++i;
        }else if(op=="write"){   // 写内存指令 // Memory write instruction
            int x=readInt(i,pos);
			int y=readInt(i,pos);
            CHECK_ADDR(x);
            m[x]=y;  // 直接写入数值 // Direct value write
            ++i;
        }else if(op=="copy"){    // 内存拷贝指令 // Memory copy instruction
            int x=readInt(i,pos);
			int y=readInt(i,pos);
            CHECK_ADDR(x);
			CHECK_ADDR(y);
            m[y]=m[x];  // 内存到内存拷贝 // Memory-to-memory copy
            ++i;
        }else if(op=="calc"){    // 计算指令 // Calculation instruction
            string mode=readStr(i,pos);
            int x=readInt(i,pos);
			int y=readInt(i,pos);
			int z=(mode=="not"?0:readInt(i,pos)); // not操作只需两个操作数 // NOT operation needs 2 operands
            
            // 地址有效性检查 
			// Address validation
            CHECK_ADDR(x);
			CHECK_ADDR(y);
			if(mode!="not"){
				CHECK_ADDR(z);
			}
            
            // 执行具体计算 
            // Perform actual calculation
            x=m[x];  // 获取内存值 // Get memory values
			y=m[y];
            if(mode=="add"){       // 加法 // Addition
				m[z]=x+y;
			}else if(mode=="sub"){ // 减法 // Subtraction
				m[z]=x-y;
			}else if(mode=="mul"){ // 乘法 // Multiplication
				m[z]=x*y;
			}else if(mode=="div"){ // 除法 // Division
                if(y==0){         // 除零检查 // Division by zero check
					cerr<<"Error(line"<<i<<"): Divide by zero\n";
					break;
				}
                m[z]=x/y;
            }else if(mode=="mod"){ // 取模 // Modulo
                if(y==0){         // 模零检查 // Modulo by zero check 
					cerr<<"Error(line"<<i<<"): Mod by zero\n";
					break;
				}
                m[z]=x%y;
            }else if(mode=="and"){ // 按位与 Bitwise AND
				m[z]=x&y;
			}else if(mode=="or"){  // 按位或 Bitwise OR
				m[z]=x|y;
			}else if(mode=="xor"){ // 按位异或 Bitwise XOR
				m[z]=x^y;
			}else if(mode=="shl"){ // 左移 Left shift
			    if(y<0){          // 负位移检查 Negative shift check
			        cerr<<"Error(line"<<i<<"): Negative shift\n";
			        break;
			    }
				m[z]=x<<y;
			}else if(mode=="shr"){ // 右移 // Right shift
			    if(y<0){          // 负位移检查 // Negative shift check
			        cerr<<"Error(line"<<i<<"): Negative shift\n";
			        break;
			    }				
				m[z]=x>>y;
			}else if(mode=="not"){ // 按位取反 // Bitwise NOT
				m[y]=~x;
			}else{
				cerr<<"Error(line"<<i<<"): Invalid calc\n";
				break;
			}
            ++i;
        }else if(op=="goto"){    // 无条件跳转 // Unconditional jump
            int x=readInt(i,pos);
            if(x<1||x>code_lines){ // 行号有效性检查 // Line number validation
				cerr<<"Error(line"<<i<<"): Invalid jump\n";
				break;
			}
            i=x-1;  // 跳转到指定行（数组从0开始） // Jump to target line (0-based index)
        }else if(op=="if"){      // 条件跳转 // Conditional jump
            string mode=readStr(i,pos);
            int x=readInt(i,pos);
			int y=readInt(i,pos);
			int z=readInt(i,pos);
            
            // 地址有效性检查 
            // Address validation
            CHECK_ADDR(x);
			CHECK_ADDR(y);
            
            // 获取内存值 
            // Get memory values
            x=m[x];
			y=m[y];
            
            // 条件判断 
            // Condition evaluation
            bool cond=(mode=="<"?(x<y):mode==">"?(x>y):mode=="=="?(x==y):mode=="<="?(x<=y):mode==">="?(x>=y):mode=="!="?(x!=y):false);
            if(!cond){
				++i;  // 条件不满足时顺序执行 // Sequential execution if condition not met
				continue;
			}
            
            // 跳转目标检查 
			// Jump target validation
            if(z<1||z>code_lines){
				cerr<<"Error(line"<<i<<"): Invalid jump\n";
				break;
			}
            i=z-1;  // 执行跳转 // Execute jump
        }else if(op=="exit"){    // 退出指令 // Exit instruction
            break;
        }else{                   // 无效操作码处理 // Invalid opcode handling
            cerr<<"Error(line"<<i<<"): Invalid opcode\n";
            break;
        }
    }
    cout<<"\n\n";
    system("pause");
    return 0;
}
