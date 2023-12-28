/**
 * 
    gcc -I/usr/local/include -o fz_ink_annot_example \
	/home/weikan/git/mupdf-android-fitz/libmupdf/docs/examples/fz_ink_annot_example.c \
	/usr/local/lib/libmupdf.a \
	/usr/local/lib/libmupdf-third.a \
	-lm
./fz_ink_annot_example document.pdf
*/



#include <mupdf/fitz.h>
#include <mupdf/pdf/document.h>
#include <mupdf/pdf/annot.h>
#include <mupdf/pdf/crypt.h>

#include <stdio.h>
#include <time.h>

int main(int argc, char** argv)
{
    char* input = argv[1];

    fz_context *ctx;
    fz_document *doc;

    ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
    fz_try(ctx)
    {
        fz_register_document_handlers(ctx);
    }
    fz_catch(ctx)
    {
        fz_drop_context(ctx);
        return -1;
    }

    fz_try(ctx)
    {
        doc = fz_open_document(ctx, input);
    }
    fz_catch(ctx)
    {
        fz_drop_context(ctx);
        return -1;
    }

    fz_try(ctx)
    {
        pdf_document *pdfdoc = pdf_document_from_fz_document(ctx, doc);
        printf("load pdfdoc\n");
        fz_page *page = fz_load_page(ctx, doc, 0);  
        printf("load pdfpage\n");
        pdf_page *pdfpage = pdf_page_from_fz_page(ctx, page);
        pdf_annot *annot = pdf_create_annot(ctx, pdfpage, PDF_ANNOT_INK);
        float color[3]={255,0,0};
        pdf_set_annot_color(ctx, annot, 3, color);
        pdf_set_annot_border_style(ctx, annot, PDF_BORDER_STYLE_SOLID);
        pdf_set_annot_border_width(ctx, annot, 10);
        int n =4;
        int count[4]={2,2,2,2};
        fz_point points[] = {{50,50}, {100, 50}, {150,50}, {150,100}, {50,50}, {100, 100}, {50,150}, {0,100}};
        pdf_set_annot_ink_list(ctx, annot, n, count, points);

        pdf_annot *annot1 = pdf_create_annot(ctx, pdfpage, PDF_ANNOT_INK);
        float color1[3]={90,155,0};
        pdf_set_annot_color(ctx, annot1, 3, color1);
        pdf_set_annot_border_style(ctx, annot1, PDF_BORDER_STYLE_SOLID);
        pdf_set_annot_border_width(ctx, annot1, 2);
        int n1 =4;
        int count1[4]={2,2,2,2};
        fz_point points1[] = {{150,150}, {200, 150}, {250,150}, {250,200}, {150,150}, {200, 200}, {150,250}, {200,400}};
        pdf_set_annot_ink_list(ctx, annot1, n1, count1, points1);

        if(pdf_update_page(ctx, pdfpage)) {
            printf("pdf page updated\n");
        }
        pdf_write_options pdfoptions = pdf_default_write_options;
        if(pdf_can_be_saved_incrementally(ctx, pdfdoc)) {
            pdfoptions.do_incremental = 1;
        }
        // if(pdf_can_be_saved_incrementally){
        //     pdfoptions.do_incremental = 1;       
        //     pdfoptions.do_linear = 0;     
        // }                     
        pdf_save_document(ctx, pdfdoc, "/home/weikan/resources/ink_annot_fix.pdf", &pdfoptions);
        
        fz_drop_page(ctx, page);    
                    
    }
    fz_catch(ctx)
    {    
        fz_log_error(ctx, fz_caught_message(ctx));
    }
    
    
    fz_drop_document(ctx,doc);
    fz_drop_context(ctx);        
    
    return 0;
}