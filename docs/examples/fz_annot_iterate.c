/**
 * 
    gcc -I/usr/local/include -o fz_annot_iterate \
	/home/weikan/git/mupdf-android-fitz/libmupdf/docs/examples/fz_annot_iterate.c \
	/usr/local/lib/libmupdf.a \
	/usr/local/lib/libmupdf-third.a \
	-lm
./fz_annot_iterate document.pdf
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
        pdf_annot* annot = pdf_first_annot(ctx, pdfpage);
        do {
            const char* contents = pdf_annot_contents(ctx, annot);
            printf("annotation content:%s", contents);
            if(pdf_annot_has_rect(ctx, annot)) {
                fz_rect rect = pdf_annot_rect(ctx, annot);
                printf(" ,rect(%f,%f,%f,%f)", rect.x0, rect.y0, rect.x1, rect.y1);
            }
            if(pdf_annot_has_author(ctx, annot)) {
                printf(", author:%s", pdf_annot_author(ctx,annot));
            }
            if(pdf_annot_has_quad_points(ctx, annot)) {
                printf("quadpoint count :%d", pdf_annot_quad_point_count(ctx, annot));
                // int count = pdf_annot_quad_point_count(ctx, annot);
                // for(int i=0;i<count;i++) {
                //     fz_quad quadp = pdf_annot_quad_point(ctx, annot, i);
                //     printf(", %i quadpoint(%d,%d,%d,%d)", quadp.ll, quadp.lr, quadp.ul, quadp.ur);
                // }
            }        
            printf("\n");
        }while(annot = pdf_next_annot(ctx, annot));
        
        
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