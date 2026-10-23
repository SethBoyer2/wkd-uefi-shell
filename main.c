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


void Strip(CHAR16 *Buffer){
	UINTN start = 0;
	UINTN end = 0;

	// Find first character that isn't a space
	while (Buffer[start] == L' ') {
		start++;
	}
	// if the whole string was spaces, clear & return
	if (Buffer[start] == L'\0') {
		Buffer[0] = L'\0';
		return;
	}
	// find end of string
	while (Buffer[end] != L'\0') {
		end++;
	}
	end--; // step to character before null terminator

	// backtrack to drop trailing spaces
	while(end >= start && Buffer[end] == L' ') {
		end--;
	}

	// shift characters to the front of the buffer
	UINTN i = 0;
	while (start <= end) {
		Buffer[i] = Buffer[start];
		i++;
		start++;
	}

	// cap off buffer w/ null terminator
	Buffer[i] = L'\0';
}

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

void print(CHAR16* String) {
	ST->ConOut->OutputString(ST->ConOut, String);
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
	print(L"-------------------------------\r\n");
	print(L"Wicked Firmware Shell 1.0.1\r\n");
	print(L"-------------------------------\r\n");
	print(L"Available Commands: \r\n help \r\n clear\r\n\r\n");
	print(L"WKD:> ");


	/* Clear system buffer to prevent accidental keypresses */
	Status = ST->ConIn->Reset(ST->ConIn, FALSE);
    if (EFI_ERROR(Status))
        return Status;

	// Declare loop
	while (1)
	{
    	ST->ConIn->ReadKeyStroke( // Read input
			ST->ConIn,
			&Key
		);

		// HANDLE ENTER
		if (Key.UnicodeChar == L'\r') 
		{
			Strip(Buffer);
			// keep comparisons above buffer reset
			if (StringsEqual(Buffer, CommandNames[CMD_HELP], 4)) {
				print(L"\r\n'clear' | Clears screen\r\n'info'  | Displays basic WKD shell information\r\n");

			} else if (StringsEqual(Buffer, CommandNames[CMD_CLEAR], 5)) {
				cls();

			} else if (StringsEqual(Buffer, CommandNames[CMD_INFO], 4)) {
				print(L"\r\n--- WKD Shell ---\r\n");
				print(L"\r\n| The WKD shell is an experimental EFI shell, written using GNU-EFI without most of the basic libraries/in-built functions, running python-like, purpose-built  helper functions|\r\n");

			} else{
				print(L"\r\nUnrecognized Command: ");
				print(Buffer);
				print(L"\r\n");
			}


			// This creates a new line. Keep this above buffer reset and below everything else
			print(L"\r\n");
			print(L"WKD:> "); // print the prompt at each new line

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
