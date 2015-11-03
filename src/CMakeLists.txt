#INCLUDE_DIRECTORIES(
#  ../include
#  )

ADD_EXECUTABLE(mpi_test mpi_hello.cpp)
ADD_EXECUTABLE(mpi_getg mpi_getg.cpp)
ADD_EXECUTABLE(mpi_bcast mpi_bcast.cpp calfunc.h)
ADD_EXECUTABLE(mpi_RTTtest mpi_RTTtest.cpp calfunc.h)
ADD_EXECUTABLE(mpi_send_recv mpi_send_recv.cpp)
ADD_EXECUTABLE(mpi_gather mpi_gather.cpp calfunc.h)
ADD_EXECUTABLE(mpi_reduce mpi_reduce.cpp calfunc.h)
ADD_EXECUTABLE(mpi_allreduce mpi_allreduce.cpp calfunc.h)

TARGET_LINK_LIBRARIES(mpi_test
   ${MPI_LIBRARIES}
   )
TARGET_LINK_LIBRARIES(mpi_getg 
   ${MPI_LIBRARIES}
   )
TARGET_LINK_LIBRARIES(mpi_bcast 
   ${MPI_LIBRARIES}
   )
TARGET_LINK_LIBRARIES(mpi_RTTtest ${MPI_LIBRARIES})
TARGET_LINK_LIBRARIES(mpi_send_recv ${MPI_LIBRARIES})
TARGET_LINK_LIBRARIES(mpi_gather ${MPI_LIBRARIES})
TARGET_LINK_LIBRARIES(mpi_reduce ${MPI_LIBRARIES})
TARGET_LINK_LIBRARIES(mpi_allreduce ${MPI_LIBRARIES})
