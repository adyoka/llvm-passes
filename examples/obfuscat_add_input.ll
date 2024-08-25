; ModuleID = './inputs/arith_obf_add_input.c'
source_filename = "./inputs/arith_obf_add_input.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx13.0.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind ssp willreturn memory(none) uwtable
define noundef signext i8 @foo(i8 noundef signext %0, i8 noundef signext %1, i8 noundef signext %2, i8 noundef signext %3) local_unnamed_addr #0 {
  %5 = add i8 %1, %0
  %6 = add i8 %5, %2
  %7 = add i8 %6, %3
  ret i8 %7
}

; Function Attrs: mustprogress nofree nounwind ssp willreturn memory(read) uwtable
define i32 @main(i32 noundef %0, ptr nocapture noundef readonly %1) local_unnamed_addr #1 {
  %3 = getelementptr inbounds ptr, ptr %1, i64 1
  %4 = load ptr, ptr %3, align 8, !tbaa !5
  %5 = tail call i32 @atoi(ptr nocapture noundef %4)
  %6 = getelementptr inbounds ptr, ptr %1, i64 2
  %7 = load ptr, ptr %6, align 8, !tbaa !5
  %8 = tail call i32 @atoi(ptr nocapture noundef %7)
  %9 = getelementptr inbounds ptr, ptr %1, i64 3
  %10 = load ptr, ptr %9, align 8, !tbaa !5
  %11 = tail call i32 @atoi(ptr nocapture noundef %10)
  %12 = getelementptr inbounds ptr, ptr %1, i64 4
  %13 = load ptr, ptr %12, align 8, !tbaa !5
  %14 = tail call i32 @atoi(ptr nocapture noundef %13)
  %15 = add i32 %8, %5
  %16 = add i32 %15, %11
  %17 = add i32 %16, %14
  %18 = shl i32 %17, 24
  %19 = ashr exact i32 %18, 24
  ret i32 %19
}

; Function Attrs: mustprogress nofree nounwind willreturn memory(read)
declare i32 @atoi(ptr nocapture noundef) local_unnamed_addr #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind ssp willreturn memory(none) uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cmov,+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree nounwind ssp willreturn memory(read) uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cmov,+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nofree nounwind willreturn memory(read) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cmov,+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 2}
!4 = !{!"Homebrew clang version 18.1.8"}
!5 = !{!6, !6, i64 0}
!6 = !{!"any pointer", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
