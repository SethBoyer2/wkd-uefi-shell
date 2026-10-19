#include <efi.h>
#include <efilib.h>


typedef enum {
    CMD_HELP,
    CMD_CLEAR,
    CMD_INFO,
    CMD_EXIT,
    CMD_COUNT
} Command;

const CHAR16 *CommandNames[CMD_COUNT] = {
    L"help",
    L"clear",
    L"info",
    L"exit"
};

/* 
 arg txtLim = maximum permitted string length to compare, excluding null terminator
 ex. StringsEqual(Buffer, CommandName[CMD_HELP], 4), 

 Returns: Bool TRUE or FALSE
 Args: *A: CHAR16 null-terminated string, *B: CHAR16 null-terminated string, txtLim: UINTN
 Compares value @ index of both arrays, returns false when mismatch is found
*/
BOOLEAN StringsEqualCase(const CHAR16 *A, const CHAR16 *B, const UINTN txtLim) {
	UINTN i = 0;
	for (i = 0; i < txtLim+1; i++) { // Txtlim +1 to account for null terminator
		if(A[i] != B[i]) {
			return FALSE;

		}else if (A[i] == L'\0' && B[i] == L'\0') {
			return TRUE;
		} 
	}
	return FALSE;
}

/*
 Case-insensitive version of StringsEqualCase
*/
BOOLEAN StringsEqual(const CHAR16 *A, const CHAR16 *B, const UINTN txtLim) {
	UINTN i = 0;
	for (i = 0; i < txtLim+1; i++) { // Txtlim +1 to account for null terminator
		// Declare temporary variables to store compared values in
		CHAR16 Atemp = A[i];
		CHAR16 Btemp = B[i];

		// if either one is uppercase, normalize to lower
		if (A[i] >= L'A' && A[i] <= L'Z'){
			Atemp = A[i] + (L'a' - L'A');
		} 
		if (B[i] >= L'A' && B[i] <= L'Z'){
			Btemp = B[i] + (L'a' - L'A');
		}

		if(Atemp != Btemp) {
			return FALSE;

		}else if (Atemp == L'\0' && Btemp == L'\0') {
			return TRUE;
		} 
	}
	return FALSE;
}

/*
Need a strip() function for space ignoring for commands

need to accept buffer, strip it, and return buffer
*/
// void Strip(CHAR16 *Buffer, Position){
// 	for (UINTN i = 0; i < Position; i++){
// 		if (Buffer[i] == L' ') {
// 			Buffer[i] = L'\0';
// 		}

// 	}
// }

/*
	Returns number of iterations through a string until null terminator is found
	Args: CHAR16 *Str, UINTN lim
*/
UINTN Len(const CHAR16 *Str, const UINTN lim){ // Iterate until null terminator is found, return length var
	UINTN len = 0;
	for (UINTN i = 0; i < lim; i++) {
		if (Str[i] == L'\0') {
			return len;
		} 
		len++;
	}
}

// little helper function. clears the screen, that's all.
void cls(void) {
	ST->ConOut->ClearScreen(ST->ConOut);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	/* hold EFI status and input key*/
	EFI_STATUS Status;
	EFI_INPUT_KEY Key;

	// i use this buffer for the console commands
	CHAR16 Buffer[32] = {0};
	UINTN Position = 0;

	/*store system table for use in other functions*/
	ST = SystemTable;

	/* clear efi boot msgs & output title text */
	cls();
	ST->ConOut->OutputString(ST->ConOut, L"-------------------------------\r\n");
	ST->ConOut->OutputString(ST->ConOut, L"Wicked Firmware Shell 1.0.1\r\n");
	ST->ConOut->OutputString(ST->ConOut, L"-------------------------------\r\n");
	ST->ConOut->OutputString(ST->ConOut, L"Available Commands: \r\n help \r\n clear\r\n\r\n");
	ST->ConOut->OutputString(ST->ConOut, L"WKD:> ");


	/* Clear system buffer to prevent accidental keypresses */
	Status = ST->ConIn->Reset(ST->ConIn, FALSE);
    if (EFI_ERROR(Status))
        return Status;

	// Declare loop
	while (1)
	{
    	Status = ST->ConIn->ReadKeyStroke( // Read input
			ST->ConIn,
			&Key
		);

		// HANDLE ENTER
		if (Key.UnicodeChar == L'\r') // If character = linebreak (enter) exit program
		{

			if (Len(Buffer, 32) == 4) {
				ST->ConOut->OutputString(ST->ConOut, L"wooo it worked");
			};

			//Strip(Buffer, Position);
			// keep comparisons above buffer reset
			if (StringsEqual(Buffer, CommandNames[CMD_HELP], 4)) {
				Status = ST->ConOut->OutputString(ST->ConOut, L"\r\nFull Match\r\n");
				if (EFI_ERROR(Status)) {
					return Status;
				}
			} else if (StringsEqual(Buffer, CommandNames[CMD_CLEAR], 5)) {
				cls();
			} else{
				ST->ConOut->OutputString(ST->ConOut, L"\r\nUnrecognized Command: ");
				ST->ConOut->OutputString(ST->ConOut, Buffer);
				ST->ConOut->OutputString(ST->ConOut, L"\r\n");
			}

			// This creates a new line. Keep this above buffer reset and below everything else
			ST->ConOut->OutputString(ST->ConOut, L"\r\n");
			ST->ConOut->OutputString(ST->ConOut, L"WKD:> "); // print the prompt at each new line

			// This resets the buffer. Keep this at the bottom of this conditional
			Position = 0;
			Buffer[Position] = L'\0';


		};

		if (Status == EFI_NOT_READY) // If no keypress, continue loop
			continue;

		if (EFI_ERROR(Status)) // Detect keyboard input errors
		{
			ST->ConOut->OutputString(
				ST->ConOut,
				L"KEYBOARD ERROR\r\n"
			);

			while (1) { }
		}

		// If backspace, decrement index and terminate entry
		if (Key.UnicodeChar == L'\b' && Position>0) {
			Position--;
			Buffer[Position] = L'\0';

			Status = ST->ConOut->OutputString(ST->ConOut, L"\b \b");

			if (EFI_ERROR(Status))
				return Status;
			continue;
		}

		if (Key.UnicodeChar >= L' ' &&
		Key.UnicodeChar <= L'~' &&
		Position < 31)
		{
			Buffer[Position] = Key.UnicodeChar;
			Position++;
			Buffer[Position] = L'\0';

			// Echo with a size of 2, with the echo containing the Key pressed, and a null terminator
			CHAR16 Echo[2] = {
				Key.UnicodeChar,
				L'\0'
			};

			// Output the echo to the console
			Status = ST->ConOut->OutputString(
				ST->ConOut,
				Echo
			);

			if (EFI_ERROR(Status))
				return Status;
		}
	}
	return EFI_SUCCESS;
}
