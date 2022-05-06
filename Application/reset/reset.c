#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleTextInEx.h>
void showHelp(){
    printf("===================================HELP================================\n");
    printf("-c ||-c causes a system-wide reset. This sets all circuitry within the \nsystem to its initial state.\n");
    printf("-w ||-w causes a system-wide initialization. The processors are set to \ntheir initial state, and pending cycles are not corrupted.\n");
    printf("-s ||-s causes the system to enter a power state equivalent to the ACPI\n G2/S5 or G3 states.\n ");
    printf("-t ||whatever you select,you should add -t to set time to wait for reset.\n");
    printf("example:reset.efi -c -t 20\n");
    printf("-h ||-h show how to use\n");
	printf("===================================HELP================================\n");
}
int getMode(char *p,char *t){//得到重启模式 1为冷启动，2为热启动 3为shutdown
    int len1=strlen(p),len2=strlen(t);
    int mode=0;
    if(len2!=2||t[0]!='-'||t[1]!='t'){
        return 0;
    }
    if(len1!=2||p[0]!='-'){
        return 0;
    }
    switch (p[1])
    {
    case 'c':
        mode=1;
        break;
    case 'w':
        mode=2;
        break;
    case 's':
        mode=3;
        break;
    default:
        break;
    }
    return mode;
}
EFI_STATUS waitTime(char *p){
        EFI_STATUS Status;
        UINTN index=0;
        long int time = 0;
        time = strtol(p, NULL, 10);
        EFI_EVENT event;
        Status=gBS->CreateEvent(EVT_TIMER,TPL_APPLICATION,(EFI_EVENT_NOTIFY)NULL,(VOID *)NULL,&event);
        Status=gBS->SetTimer(event,TimerPeriodic,10*1000*1000);
        while (1)
        {
            Status=gBS->WaitForEvent(1,&event,&index);
            Print(L"Time Wait %d\n",time);
            time--;
            if(time<0){
                break;
            }
        }
        Status = gBS->CloseEvent(event);
        return Status;
}
EFI_STATUS reset(char *p,int n){
    if(n==1){
        waitTime(p);
        gRT->ResetSystem(EfiResetCold, EFI_SUCCESS,0,NULL);
    }else if(n==2){
        waitTime(p);
        gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS,0,NULL);
    }else if(n==3){
        waitTime(p);
        gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS,0,NULL);
    }else{
        showHelp();
    }
    return EFI_SUCCESS;
}
int main(
        int        Argc,
        char     **Argv
		)
{
    EFI_STATUS Status;
    if(Argc!=4){
        showHelp();
        return 0;
    }
    int mode=getMode(Argv[1],Argv[2]);
    if(mode==0){
        showHelp();
        return 0;
    }else{
        Status=reset(Argv[3],mode);
    }
    //Status=testMouseSimple();
	return 0;
}